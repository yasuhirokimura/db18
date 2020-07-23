/*
 * Copyright (c) 2016, 2019 Oracle and/or its affiliates.  All rights reserved.
 *
 * See the file LICENSE for license information.
 *
 * $Id$
 */

import UIKit

class ViewController: UIViewController {
    @IBOutlet weak var dbVersionLabel: UILabel!

    @IBOutlet weak var sqlVersionLabel: UILabel!

    @IBOutlet weak var resultLabel: UILabel!

    @IBOutlet weak var startTestButton: UIButton!

    @IBOutlet weak var keyStoreEnabled: UISwitch!

    @IBOutlet weak var messageView: UITextView!

    override func viewDidLoad() {
        super.viewDidLoad()

        dbVersionLabel.text = SQLiteDb.dbVersionText
        sqlVersionLabel.text = SQLiteDb.sqliteVersionText

        messageView.layer.borderWidth = 0.5
        messageView.layer.borderColor = UIColor.lightGrayColor().CGColor
        messageView.layer.cornerRadius = 5.0

        if (!SQLiteDb.authenticationEnabled) {
            startTestButton.enabled = false
            resultLabel.text = "Authentication is disabled!"
            resultLabel.textColor = UIColor.redColor()
            resultLabel.hidden = false
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    @IBAction func startTest(sender: UIButton) {
        do {
            let tests = AuthenticationTests()
            tests.messageListener = {
                self.messageView.text = self.messageView.text + $0
            }

            try tests.runAuthenticationTests(keyStoreEnabled.on)
            if (keyStoreEnabled.on) {
                try tests.runKeyStoreLockFileTests()
            }
        } catch let e as TestError {
            reportError(e.description)
        } catch let e as NSError {
            reportError(e.description)
        }

        resultLabel.hidden = false
    }

    func reportError(errMessage: String) {
        messageView.text = "\(messageView.text)\tFailed.\n\t\(errMessage)"
        resultLabel.text = "Test Failed!"
        resultLabel.textColor = UIColor.redColor()
    }

}

