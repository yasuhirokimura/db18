/*
 * Copyright (c) 2016, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

import Foundation

/**
 This class provides various SQLite authentication tests
 */
class AuthenticationTests {
    /** The URL of the database file. */
    private let testDatabaseURL: NSURL

    /** The URL of the database journal directory. */
    private let journalDirURL: NSURL

    /** The URL of the key store file. */
    private let keyStoreFileURL: NSURL

    /** The URL of the key store lock file. */
    private let keyStoreLockFileURL: NSURL

    /** The listener of test progress messages. */
    var messageListener: (String) -> () = { _ in }

    init() {
        let documentDir = NSFileManager.defaultManager()
            .URLsForDirectory(.DocumentDirectory,
                              inDomains: .UserDomainMask)[0]

        let testDatabaseName = "test.db"

        testDatabaseURL = documentDir
            .URLByAppendingPathComponent(testDatabaseName)

        journalDirURL = documentDir
            .URLByAppendingPathComponent(testDatabaseName + "-journal")

        keyStoreFileURL = journalDirURL
            .URLByAppendingPathComponent("sql-userauth.ks")

        keyStoreLockFileURL = journalDirURL
            .URLByAppendingPathComponent("___sql-userauth.ks.lck")
    }

    /**
     A suite of tests for the SQLite user authentication feature.

     - parameter useKeyStore: if keystore is enabled at build time
     */
    func runAuthenticationTests(useKeyStore: Bool) throws {
        messageListener("==== User authentication tests start ====\n")

        messageListener("\n== a. Create an authentication-required database ==\n")
        messageListener("Clean up test environment... ")
        try cleanUpTestEnvironment()
        messageListener("\t Done.\n")

        messageListener("Connect to the test database... ")
        var testDb = try SQLiteDb(fileName: testDatabaseURL.path!)
        messageListener("\t OK.\n")

        if (SQLiteDb.encryptionEnabled) {
            try assert(expected: [[String]](),
                       actual: testDb.executeSQL("PRAGMA key=\"testKey\""),
                       info: "Set the encryption key")
        }

        try assert(expected: false,
                   actual: userTableExists(testDb),
                   info: "Check that table \"sqlite_user\" does not exist")

        try assert(expected: (),
                   actual: testDb.addUser("Alice", password: "pwd"),
                   info: "Add the first user \"Alice\"")

        try assert(expected: true,
                   actual: userTableExists(testDb),
                   info: "Check that table \"sqlite_user\" is created")

        try assert(expected: [("Alice", true)],
                   actual: listUsers(testDb),
                   info: "Check that user \"Alice\" exists")

        if (useKeyStore) {
            try assert(expected: true,
                       actual: NSFileManager.defaultManager()
                                .fileExistsAtPath(keyStoreFileURL.path!),
                       info: "Check that the key store file is created")
        }

        messageListener("Close the connection.\n")
        testDb.close()

        messageListener("\n== b. Access the database without logging in ==\n")
        messageListener("Connect to the test database... ")
        testDb = try SQLiteDb(fileName: testDatabaseURL.path!)
        messageListener("\t OK.\n")

        try assertAuthError("Check that the database cannot be accessed",
                            encryptionEnabled: SQLiteDb.encryptionEnabled) {
            try self.userTableExists(testDb)
        }

        messageListener("Reconnect to the test database... ")
        testDb.close()
        testDb = try SQLiteDb(fileName: testDatabaseURL.path!)
        messageListener("\t OK.\n")

        try assertAuthError("Check that adding user fails",
                            encryptionEnabled: SQLiteDb.encryptionEnabled) {
            try self.userTableExists(testDb)
        }

        messageListener("Close the connection.\n")
        testDb.close()

        messageListener("\n== c. Access the database after logging in ==\n")
        messageListener("Connect to the test database... ")
        testDb = try SQLiteDb(fileName: testDatabaseURL.path!)
        messageListener("\t OK.\n")

        if (SQLiteDb.encryptionEnabled && !useKeyStore) {
            try assert(expected: [[String]](),
                       actual: testDb.executeSQL("PRAGMA key=\"testKey\""),
                       info: "Set the encryption key")
        }

        try assert(expected: (),
                   actual: testDb.authenticateUser("Alice", password: "pwd"),
                   info: "Log in as \"Alice\"")

        try assert(expected: [("Alice", true)],
                   actual: listUsers(testDb),
                   info: "Check that the database can be accessed now")

        messageListener("\n== d. Test user management ==\n")
        try assert(expected: (),
                   actual: testDb.addUser("Bob", password: "pwd", admin: false),
                   info: "Add the second user \"Bob\"")

        try assert(expected: [("Alice", true), ("Bob", false)],
                   actual: listUsers(testDb),
                   info: "Check that there are two users now")

        try assert(expected: (),
                   actual: testDb.changeUser("Bob", password: "p", admin: true),
                   info: "Make \"Bob\" an administrator")

        try assert(expected: [("Alice", true), ("Bob", true)],
                   actual: listUsers(testDb),
                   info: "Check that \"Bob\" is an administrator now")

        try assert(expected: (),
                   actual: testDb.authenticateUser("Bob", password: "p"),
                   info: "Log in as \"Bob\" with changed password")

        try assertAuthError("Check that the current user cannot be deleted") {
            try testDb.deleteUser("Bob")
        }

        try assert(expected: (),
                   actual: testDb.authenticateUser("Alice", password: "pwd"),
                   info: "Switch to \"Alice\"")

        try assert(expected: (),
                   actual: testDb.deleteUser("Bob"),
                   info: "Delete \"Bob\"")

        try assert(expected: [("Alice", true)],
                   actual: listUsers(testDb),
                   info: "Check that \"Bob\" is deleted")

        try assertAuthError("Check that the last user cannot be deleted") {
            try testDb.deleteUser("Alice")
        }

        messageListener("Close the connection.\n")
        testDb.close()
    }

    /**
     A suite of tests for user authentication keystore lock file.
     */
    func runKeyStoreLockFileTests() throws {
        messageListener("\n\n==== Keystore lock file tests start ====\n")

        messageListener("\n== a. Log in should fail if the lock file exists ==\n")
        try assert(expected: true,
                   actual: createLockFile(),
                   info: "Create the lock file")

        messageListener("Connect to the test database... ")
        let testDb = try SQLiteDb(fileName: testDatabaseURL.path!)
        messageListener("\t OK.\n")

        try assertAuthError("Check that logging in fails") {
            try testDb.authenticateUser("Alice", password: "pwd")
        }

        messageListener("\n== b. Remove the lock file and login again ==\n")
        try assert(expected: (),
                   actual: removeLockFile(),
                   info: "Remove the lock file")

        try assert(expected: (),
                   actual: testDb.authenticateUser("Alice", password: "pwd"),
                   info: "Log in as \"Alice\"")

        try assert(expected: (),
                   actual: testDb.addUser("Bob", password: "pwd", admin: false),
                   info: "Add the second user \"Bob\"")

        messageListener("\n== c. User management should fail if the lock file exists ==\n")
        try assert(expected: true,
                   actual: createLockFile(),
                   info: "Create the lock file")

        try assertAuthError("Check that adding user fails") {
            try testDb.addUser("Cindy", password: "pwd")
        }

        try assertAuthError("Check that changing user info fails") {
            try testDb.changeUser("Bob", password: "newPwd", admin: false)
        }

        try assertAuthError("Check that deleting user fails") {
            try testDb.deleteUser("Bob")
        }

        messageListener("Close the connection.\n")
        testDb.close()
    }

    func cleanUpTestEnvironment() throws {
        let fileManager = NSFileManager.defaultManager()

        if (fileManager.fileExistsAtPath(testDatabaseURL.path!)) {
            try fileManager.removeItemAtURL(testDatabaseURL)
            try fileManager.removeItemAtURL(journalDirURL)
        }
    }

    func userTableExists(db: SQLiteDb) throws -> Bool {
        let resultSet = try db.executeSQL("SELECT name FROM sqlite_master " +
            "WHERE type=\'table\' AND name=\'sqlite_user\'")

        return resultSet.count != 0
    }

    func listUsers(db: SQLiteDb) throws -> [(String, Bool)] {
        let resultSet = try db.executeSQL("SELECT uname, isadmin " +
            "FROM sqlite_user ORDER BY uname")

        return resultSet.map({ ($0[0], $0[1] == "1") })
    }

    func createLockFile() -> Bool {
        return NSFileManager.defaultManager()
            .createFileAtPath(keyStoreLockFileURL.path!,
                              contents: nil, attributes: nil)
    }

    func removeLockFile() throws {
        try NSFileManager.defaultManager().removeItemAtURL(keyStoreLockFileURL)
    }

    /**
     Check that the actual result is equal to the expected result. Throw an
     TestError.AssertError if the two are different.

     - parameter expected: the expected value
     - parameter actual: the actual value
     - parameter info: a text String explaining the test
     */
    func assert(expected expected: Any, actual: Any, info: String) throws {
        messageListener("\(info)... ")
        let expectedAsString = String(expected)
        let actualAsString = String(actual)
        guard expectedAsString == actualAsString else {
            throw TestError.AssertError(expected: expectedAsString,
                                        actual: actualAsString)
        }
        messageListener("\t OK.\n")
    }

    /**
     Check that the specified operation throws an authentication error.

     - parameter info: a text String explaining the test
     - parameter useKeyStore: if key store is enabled at build time
     - parameter operation: the operation that should fail
     */
    func assertAuthError(info: String,
                         encryptionEnabled: Bool = true,
                         operation: () throws -> ()) throws {
        messageListener("\(info)... ")
        do {
            try operation()
            throw TestError.AssertError(expected: "fail", actual: "no error")
        } catch TestError.SQLError(let code, let message)
            where message == "user not authenticated" ||
                code == SQLiteDb.authenticationErrorCode ||
                (code == SQLiteDb.genericErrorCode && encryptionEnabled) {
            messageListener("\t OK.\n")
        }
     }
}


