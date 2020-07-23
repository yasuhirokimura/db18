/*
 * Copyright (c) 2016, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

/**
 Errors thrown by this test application.
 */
enum TestError : ErrorType {
    /** SQLite errors. */
    case SQLError(code: Int32, message: String)
    /** Test failures. */
    case AssertError(expected: String, actual: String)

    /** A text description of the error. */
    var description: String {
        switch self {
        case .SQLError(_, let message):
            return message
        case .AssertError(let expected, let actual):
            return "expected: \(expected), but got \(actual)"
        }
    }
}
