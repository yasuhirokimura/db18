/*
 * Copyright (c) 2016, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

import db.core
import db.sql

/**
 A wrapper class for manipulating a SQLite database.
 */
class SQLiteDb {

    /** Berkeley DB version. */
    static let dbVersionText = String.fromCString(db_version(nil, nil, nil))

    /** SQLite version. */
    static let sqliteVersionText = "SQLite: \(SQLITE_VERSION)"

    /** SQLite authentication is enabled at compile time. */
    static let authenticationEnabled =
        sqlite3_compileoption_used("USER_AUTHENTICATION") == 1

    /** SQLite encryption is enabled at compile time. */
    static let encryptionEnabled =
        sqlite3_compileoption_used("HAS_CODEC") == 1

    /** SQLITE error code for general errors. */
    static let genericErrorCode = SQLITE_ERROR

    /** SQLite error code for authentication errors. */
    static let authenticationErrorCode = SQLITE_AUTH

    /**
     The SQLite database pointer.

     - note:
     C Type: `sqlite3 *`
     */
    private let db: COpaquePointer

    /**
     Open a SQLite connection for a specified database.

     - parameter fileName: the file path of the database

     - throws: TestError.SQLError
     */
    init(fileName: String) throws {
        var pdb: COpaquePointer = nil

        try SQLiteDb.callSQLite(pdb) {
            sqlite3_open(fileName, &pdb)
        }

        db = pdb
    }

    /** Close this database's connection. */
    func close() {
        sqlite3_close(db)
    }

    /**
     Execute a single SQL statement and returns its result rows.

     - parameter stmt: the SQL statement

     - returns: the result rows as an array where each row is an array of
     column values as Strings

     - throws: TestError.SQLError
     */
    func executeSQL(stmt: String) throws -> [[String]] {
        var pStmt: COpaquePointer = nil

        // compile the SQL statement
        try SQLiteDb.callSQLite(db) {
            sqlite3_prepare_v2(self.db, stmt, -1, &pStmt, nil)
        }

        // close the compiled statement at function exit
        defer {
            sqlite3_finalize(pStmt)
        }

        // iterate result rows and collect each row into an array
        var queryResult = [[String]]()
        while true {
            let retCode = sqlite3_step(pStmt)
            switch retCode {
            case SQLITE_DONE:
                return queryResult;
            case SQLITE_ROW:
                // map each column index into its column value
                // and add the row into the result
                queryResult.append(
                    (0..<sqlite3_data_count(pStmt)).map() {
                        String.fromCString(UnsafePointer<CChar>(
                                sqlite3_column_text(pStmt, $0)))!
                    })
            default:
                throw TestError.SQLError(code: retCode,
                                         message: SQLiteDb.sqliteError(db))
            }
        }
    }

    /**
     Add a user to this database.

     - parameter name: the user name
     - parameter password: the password
     - parameter admin: the user is an administrator

     - throws: TestError.SQLError
     */
    func addUser(name: String, password: String, admin: Bool = true) throws {
        try SQLiteDb.callSQLite(db) {
            sqlite3_user_add(self.db, name, password,
                             Int32(password.utf8.count), admin ? 1 : 0)
        }
    }

    /**
     Change a user's login information.

     - parameter name: the name of the user to be updated
     - parameter password: the new password
     - parameter admin: the new administrator status

     - throws: TestError.SQLError
     */
    func changeUser(name: String, password: String, admin: Bool) throws {
        try SQLiteDb.callSQLite(db) {
            sqlite3_user_change(self.db, name, password,
                                Int32(password.utf8.count), admin ? 1 : 0)
        }
    }

    /**
     Delete a user from this database

     - parameter name: the name of the user to be deleted

     - throws: TestError.SQLError
     */
    func deleteUser(name: String) throws {
        try SQLiteDb.callSQLite(db) {
            sqlite3_user_delete(self.db, name)
        }
    }

    /**
     Log into this database as the specified user.

     - parameter name: the user name
     - parameter password: the password

     - throws: TestError.SQLError
     */
    func authenticateUser(name: String, password: String) throws {
        try SQLiteDb.callSQLite(db) {
            sqlite3_user_authenticate(self.db, name, password,
                                      Int32(password.utf8.count))
        }
    }

    /**
     Call a SQLite C function and if an error occurs wrap the error into a
     TestError.SQLError.

     - parameter pdb: the database pointer
     - parameter call: the call to a SQLite C function

     - throws: TestError.SQLError
     */
    private static func callSQLite(pdb: COpaquePointer,
                                   _ call: () -> Int32) throws {
        let retCode = call()
        guard retCode == SQLITE_OK else {
            let errCode = sqlite3_errcode(pdb)
            if errCode == SQLITE_OK {
                throw TestError.SQLError(code: retCode, message: "")
            } else {
                throw TestError.SQLError(code: errCode,
                                         message: sqliteError(pdb))
            }
        }
    }

    /**
     Get the text message of the last SQLite error for a specified database.

     - parameter pdb: the database pointer

     - returns: the text message of the last error
     */
    private static func sqliteError(pdb: COpaquePointer) -> String {
        return String.fromCString(sqlite3_errmsg(pdb))!
    }

}