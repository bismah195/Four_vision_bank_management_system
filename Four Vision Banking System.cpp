/*
 * Project: Four Vision Banking System
 * Developer: bismah195
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// ==========================================
//          STRUCTURE DEFINITIONS
// ==========================================

typedef struct {
    char name[50]; 
    int accountNum;
    float balance;
    char cnic[30];
    char password[50];
    float loanAmount;
    char loanType[20];
} UserAccount;

typedef struct {
    char type[20];
    float minBalance;
    float maxAmount;
    float interestRate;
} LoanCriteria;


// ==========================================
//          GLOBAL CONFIGURATIONS
// ==========================================

LoanCriteria globalLoans[] = {
    {"PersonalLoan", 5000.0,  100000.0, 0.07},
    {"HomeLoan",     20000.0, 500000.0, 0.05},
    {"BusinessLoan", 10000.0, 200000.0, 0.06}
};


// ==========================================
//          UTILITY FUNCTIONS
// ==========================================

void flushInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


int isCnicValid(const char* cnic) {
    if (strlen(cnic) != 15) return 0;

    for (int i = 0; i < 15; i++) {
        if (i == 5 || i == 13) {
            if (cnic[i] != '-') return 0;
        } else if (!isdigit(cnic[i])) {
            return 0;
        }
    }
    return 1;
}


int isPasswordStrong(const char* pass) {
    int upper = 0, lower = 0, digit = 0, special = 0;

    if (strlen(pass) < 8) return 0;

    for (int i = 0; pass[i] != '\0'; i++) {
        if (isupper(pass[i])) upper = 1;
        else if (islower(pass[i])) lower = 1;
        else if (isdigit(pass[i])) digit = 1;
        else if (ispunct(pass[i])) special = 1;
    }
    return (upper && lower && digit && special);
}


int generateAccID() {
    FILE *f = fopen("Accounts.txt", "r");
    if (!f) return 1001;

    int maxID = 1000;
    UserAccount temp;

    while (fscanf(f, "%d %s %f %s %s %s %f\n", 
           &temp.accountNum, temp.name, &temp.balance, 
           temp.cnic, temp.password, temp.loanType, &temp.loanAmount) == 7) {
        
        if (temp.accountNum > maxID) maxID = temp.accountNum;
    }

    fclose(f);
    return maxID + 1;
}


// ==========================================
//          CORE BANKING MODULES
// ==========================================

void registerUser() {
    FILE *f = fopen("Accounts.txt", "a+");
    if (!f) {
        printf("\n[Error] Database access denied.\n");
        return;
    }

    UserAccount n;

    printf("\n--- NEW ACCOUNT REGISTRATION ---\n");

    printf("Full Name (First_Last): "); 
    scanf("%s", n.name);

    printf("Initial Deposit: "); 
    scanf("%f", &n.balance);
    
    printf("CNIC (xxxxx-xxxxxxx-x): "); 
    scanf("%s", n.cnic);

    if (!isCnicValid(n.cnic)) { 
        printf("\n[!] Invalid CNIC format. Process aborted.\n"); 
        fclose(f); 
        return; 
    }

    printf("Set Password (8+ characters): "); 
    scanf("%s", n.password);

    if (!isPasswordStrong(n.password)) { 
        printf("\n[!] Password too weak. Registration failed.\n"); 
        fclose(f); 
        return; 
    }

    n.accountNum = generateAccID();
    n.loanAmount = 0.0;
    strcpy(n.loanType, "None");

    fprintf(f, "%d %s %.2f %s %s %s %.2f\n", 
            n.accountNum, n.name, n.balance, 
            n.cnic, n.password, n.loanType, n.loanAmount);

    fclose(f);
    printf("\n[Success] Account #%d created for %s.\n", n.accountNum, n.name);
}


void processTransaction(int type) {
    int id; 
    char pass[50]; 
    float amt;

    printf("\n--- TRANSACTION MODULE ---\n");
    printf("Account ID: "); 
    scanf("%d", &id);
    
    FILE *f = fopen("Accounts.txt", "r");
    FILE *t = fopen("Temp.txt", "w");
    if (!f || !t) return;

    UserAccount acc; 
    bool found = false;

    while (fscanf(f, "%d %s %f %s %s %s %f\n", 
           &acc.accountNum, acc.name, &acc.balance, 
           acc.cnic, acc.password, acc.loanType, &acc.loanAmount) == 7) {

        if (acc.accountNum == id) {
            found = true;
            printf("Enter Password: "); 
            scanf("%s", pass);

            if (strcmp(acc.password, pass) == 0) {
                printf("Enter Amount: "); 
                scanf("%f", &amt);

                if (type == 1 && amt > 0) {
                    acc.balance += amt;
                    printf("\n[Success] Amount deposited.\n");
                } 
                else if (type == 2 && amt > 0 && amt <= acc.balance) {
                    acc.balance -= amt;
                    printf("\n[Success] Withdrawal complete.\n");
                } 
                else {
                    printf("\n[Error] Invalid funds or amount.\n");
                }
            } else {
                printf("\n[Error] Authentication failed.\n");
            }
        }
        fprintf(t, "%d %s %.2f %s %s %s %.2f\n", 
                acc.accountNum, acc.name, acc.balance, 
                acc.cnic, acc.password, acc.loanType, acc.loanAmount);
    }

    fclose(f); 
    fclose(t);

    remove("Accounts.txt"); 
    rename("Temp.txt", "Accounts.txt");

    if (!found) printf("\n[!] Account ID not found in system.\n");
}


void secureTransfer() {
    int sender, receiver; 
    float amt; 
    char pass[50];

    printf("\n--- SECURE MONEY TRANSFER ---\n");
    printf("From Account ID: "); 
    scanf("%d", &sender);
    
    FILE *f = fopen("Accounts.txt", "r");
    if (!f) return;

    UserAccount acc; 
    bool senderAuth = false, receiverExists = false;
    
    // Pass 1: Verify Sender Credentials
    while (fscanf(f, "%d %s %f %s %s %s %f\n", 
           &acc.accountNum, acc.name, &acc.balance, 
           acc.cnic, acc.password, acc.loanType, &acc.loanAmount) == 7) {

        if (acc.accountNum == sender) {
            printf("Enter Security Password: "); 
            scanf("%s", pass);

            if (strcmp(acc.password, pass) == 0) {
                printf("Transfer Amount: "); 
                scanf("%f", &amt);
                if (acc.balance >= amt && amt > 0) senderAuth = true;
            }
        }
    }

    if (!senderAuth) { 
        printf("\n[Error] Access denied or insufficient funds.\n"); 
        fclose(f); 
        return; 
    }

    // Pass 2: Check Receiver Existence
    printf("To Account ID: "); 
    scanf("%d", &receiver);

    rewind(f);
    while (fscanf(f, "%d %s %f %s %s %s %f\n", 
           &acc.accountNum, acc.name, &acc.balance, 
           acc.cnic, acc.password, acc.loanType, &acc.loanAmount) == 7) {
        if (acc.accountNum == receiver) receiverExists = true;
    }

    if (!receiverExists) { 
        printf("\n[Error] Recipient ID not found. Transfer cancelled.\n"); 
        fclose(f); 
        return; 
    }

    // Pass 3: Execute Final Transaction
    rewind(f);
    FILE *t = fopen("Temp.txt", "w");

    while (fscanf(f, "%d %s %f %s %s %s %f\n", 
           &acc.accountNum, acc.name, &acc.balance, 
           acc.cnic, acc.password, acc.loanType, &acc.loanAmount) == 7) {
        
        if (acc.accountNum == sender) acc.balance -= amt;
        if (acc.accountNum == receiver) acc.balance += amt;

        fprintf(t, "%d %s %.2f %s %s %s %.2f\n", 
                acc.accountNum, acc.name, acc.balance, 
                acc.cnic, acc.password, acc.loanType, acc.loanAmount);
    }

    fclose(f); 
    fclose(t);

    remove("Accounts.txt"); 
    rename("Temp.txt", "Accounts.txt");

    printf("\n[Success] Transfer of %.2f complete.\n", amt);
}


// ==========================================
//              MAIN MENU SYSTEM
// ==========================================

int main() {
    int choice;

    while(1) {
        printf("\n************************************");
        printf("\n     FOUR VISION BANKING SYSTEM     ");
        printf("\n************************************");
        printf("\n1. Open New Account");
        printf("\n2. Cash Deposit");
        printf("\n3. Cash Withdrawal");
        printf("\n4. Transfer Money");
        printf("\n5. Loan Management");
        printf("\n6. Exit System");
        printf("\n************************************");
        printf("\nSelect Option: ");
        
        if (scanf("%d", &choice) != 1) {
            flushInput();
            printf("\n[!] Please use numeric inputs only.\n");
            continue;
        }

        switch(choice) {
            case 1: registerUser(); break;
            case 2: processTransaction(1); break;
            case 3: processTransaction(2); break;
            case 4: secureTransfer(); break;
            case 5: printf("\n[System] Redirecting to loan desk...\n"); break;
            case 6: 
                printf("\nExiting. Thank you for choosing Four Vision.\n"); 
                exit(0);
            default: 
                printf("\n[!] Invalid selection. Try again.\n");
        }
    }
    return 0;
}