#include <iostream>
#include <pqxx/pqxx> 

using namespace std;
using namespace pqxx;

void showMenu()
{
    cout << "**********MENU**********" << endl;
    cout << "1. Check balance" << endl;
    cout << "2. Deposit" << endl;
    cout << "3. Withdraw" << endl;
    cout << "4. Transfer money" << endl;
    cout << "5. Exit" << endl;
    cout << "************************" << endl;
}

void showBalance(int pin, connection &C)
{
    nontransaction N(C);
    string sql = "SELECT AMOUNT FROM USERS WHERE PIN = " + to_string(pin) + ";";
    result R(N.exec(sql));
    if (!R.empty()) {
        cout << "Balance is: " << R[0][0].as<int>() << " $" << endl;
    } else {
        cout << "User not found!" << endl;
    }
}

void depositMoney(int pin, double amount, connection &C)
{
    work W(C);
    string sql = "UPDATE USERS SET AMOUNT = AMOUNT + " + to_string(amount) + " WHERE PIN = " + to_string(pin) + ";";
    W.exec(sql);
    W.commit();
    cout << "Deposit successful!" << endl;
}

void withdrawMoney(int pin, double amount, connection &C)
{
    work W(C);
    string sql = "SELECT AMOUNT FROM USERS WHERE PIN = " + to_string(pin) + ";";
    result R(W.exec(sql));
    if (!R.empty() && R[0][0].as<int>() >= amount)
    {
        sql = "UPDATE USERS SET AMOUNT = AMOUNT - " + to_string(amount) + " WHERE PIN = " + to_string(pin) + ";";
        W.exec(sql);
        W.commit();
        cout << "Withdrawal successful!" << endl;
    }
    else
        cout << "Not enough money!" << endl;
}

void transferMoney(int senderPin, int recipientAcnumber, double amount, connection &C)
{
    work W(C);

    string sql = "SELECT AMOUNT FROM USERS WHERE ACNUMBER = " + to_string(recipientAcnumber) + ";";
    result R_recipient(W.exec(sql));
    if (R_recipient.empty())
    {
        cout << "Recipient not found!" << endl;
        return;
    }

    sql = "SELECT AMOUNT FROM USERS WHERE PIN = " + to_string(senderPin) + ";";
    result R_sender(W.exec(sql));
    if (!R_sender.empty() && R_sender[0][0].as<int>() >= amount)
    {
        sql = "UPDATE USERS SET AMOUNT = AMOUNT - " + to_string(amount) + " WHERE PIN = " + to_string(senderPin) + ";";
        W.exec(sql);
        sql = "UPDATE USERS SET AMOUNT = AMOUNT + " + to_string(amount) + " WHERE ACNUMBER = " + to_string(recipientAcnumber) + ";";
        W.exec(sql);
        W.commit();
        cout << "Transfer successful!" << endl;
    }
    else
        cout << "Not enough money!" << endl;
}

int main(int argc, char* argv[])
{
    string sql;

    try
    {
        connection C("dbname=default user=postgres password=postgres hostaddr=127.0.0.1 port=5432");

        if (C.is_open())
        {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } 
        else
        {
            cout << "Can't open database" << endl;
            return 1;
        }

        int option;
        int pin;

        cout << "Please enter your PIN: ";
        cin >> pin;

        while (true)
        {
            showMenu();
            cout << "Option: ";
            cin >> option;

            system("clear");

            switch (option)
            {
                case 1:
                    showBalance(pin, C);
                    break;
                case 2:
                {
                    double depositAmount;
                    cout << "Deposit amount: ";
                    cin >> depositAmount;
                    depositMoney(pin, depositAmount, C);
                    break;
                }
                    
                case 3:
                {
                    double withdrawAmount;
                    cout << "Withdraw amount: ";
                    cin >> withdrawAmount;
                    withdrawMoney(pin, withdrawAmount, C);
                    break;
                }
                    
                case 4:
                {
                    int acnumber;
                    double transferAmount;
                    cout << "Recipient account number: ";
                    cin >> acnumber;
                    cout << "Amount to transfer: ";
                    cin >> transferAmount;
                    transferMoney(pin, acnumber, transferAmount, C);
                    break;
                }
                    
                case 5:
                    cout << "Thank you for using the ATM!" << endl;
                    return 0;
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        }
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
        return 1;
    }
}
