#include <stdio.h>
#include <string.h>

#define MAX_USERS 20
#define MAX_SUBJECTS 20
#define MAX_NAME_LEN 30
#define MAX_DAYS 31
#define DATA_FILE "habits.dat"

typedef struct {
    char name[MAX_NAME_LEN];
    int daysDone[MAX_DAYS];
    int totalDays;
} Habit;

typedef struct {
    char name[MAX_NAME_LEN];

    Habit studyHabits[MAX_SUBJECTS];
    int studyCount;

    Habit hobbyHabits[MAX_SUBJECTS];
    int hobbyCount;
} User;

User users[MAX_USERS];
int userCount = 0;
int currentUser = -1;

// ===== FUNCTIONS =====
void createUser();
void selectUser();
void markDay();
void showSummary();
void viewSavedData();

float calculatePercentage(Habit h);
int calculateStreak(Habit h);
void showMotivation(float pct, char name[]);

void saveData();
void loadData();

// ===== MAIN =====
int main() {
    int choice;
    loadData();

    while (1) {
        printf("\n========== HABIT TRACKER ==========\n");

        printf("Current User: ");
        if (currentUser == -1) printf("None\n");
        else printf("%s\n", users[currentUser].name);

        printf("\n1. Create User\n");
        printf("2. Select User\n");
        printf("3. Mark Progress (1/0)\n");
        printf("4. Show Summary\n");
        printf("5. View Saved Data\n");
        printf("6. Save\n");
        printf("7. Exit\n");

        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: createUser(); break;
            case 2: selectUser(); break;
            case 3: markDay(); break;
            case 4: showSummary(); break;
            case 5: viewSavedData(); break;
            case 6: saveData(); printf("Saved!\n"); break;
            case 7: saveData(); return 0;
            default: printf("Invalid choice\n");
        }
    }
}

// ===== CREATE USER =====
void createUser() {
    if (userCount >= MAX_USERS) return;

    User *u = &users[userCount];

    printf("\nEnter name: ");
    getchar();
    fgets(u->name, MAX_NAME_LEN, stdin);
    u->name[strcspn(u->name, "\n")] = 0;

    printf("How many subjects? ");
    scanf("%d", &u->studyCount);

    for (int i = 0; i < u->studyCount; i++) {
        printf("Subject name: ");
        getchar();
        fgets(u->studyHabits[i].name, MAX_NAME_LEN, stdin);
        u->studyHabits[i].name[strcspn(u->studyHabits[i].name, "\n")] = 0;

        printf("Days to track (max 31): ");
        scanf("%d", &u->studyHabits[i].totalDays);

        if (u->studyHabits[i].totalDays > MAX_DAYS)
            u->studyHabits[i].totalDays = MAX_DAYS;

        for (int j = 0; j < MAX_DAYS; j++)
            u->studyHabits[i].daysDone[j] = 0;
    }

    printf("How many hobbies? ");
    scanf("%d", &u->hobbyCount);

    for (int i = 0; i < u->hobbyCount; i++) {
        printf("Hobby name: ");
        getchar();
        fgets(u->hobbyHabits[i].name, MAX_NAME_LEN, stdin);
        u->hobbyHabits[i].name[strcspn(u->hobbyHabits[i].name, "\n")] = 0;

        printf("Days to track (max 31): ");
        scanf("%d", &u->hobbyHabits[i].totalDays);

        if (u->hobbyHabits[i].totalDays > MAX_DAYS)
            u->hobbyHabits[i].totalDays = MAX_DAYS;

        for (int j = 0; j < MAX_DAYS; j++)
            u->hobbyHabits[i].daysDone[j] = 0;
    }

    userCount++;
    printf("User created!\n");
}

// ===== SELECT USER =====
void selectUser() {
    if (userCount == 0) return;

    printf("\nUsers:\n");
    for (int i = 0; i < userCount; i++)
        printf("%d. %s\n", i + 1, users[i].name);

    printf("Select user: ");
    scanf("%d", &currentUser);
    currentUser--;

    if (currentUser < 0 || currentUser >= userCount) {
        printf("Invalid\n");
        currentUser = -1;
    }
}

// ===== MARK =====
void markDay() {
    if (currentUser == -1) {
        printf("Select user first\n");
        return;
    }

    User *u = &users[currentUser];

    int type, index, day, status;

    printf("\n1. Study\n2. Hobby\nChoose: ");
    scanf("%d", &type);

    if (type == 1) {
        for (int i = 0; i < u->studyCount; i++)
            printf("%d. %s\n", i + 1, u->studyHabits[i].name);

        scanf("%d", &index);
        index--;

        Habit *h = &u->studyHabits[index];

        printf("Enter day (1-%d): ", h->totalDays);
        scanf("%d", &day);

        printf("Enter status (1=done, 0=not done): ");
        scanf("%d", &status);

        h->daysDone[day - 1] = status;
    } else {
        for (int i = 0; i < u->hobbyCount; i++)
            printf("%d. %s\n", i + 1, u->hobbyHabits[i].name);

        scanf("%d", &index);
        index--;

        Habit *h = &u->hobbyHabits[index];

        printf("Enter day (1-%d): ", h->totalDays);
        scanf("%d", &day);

        printf("Enter status (1=done, 0=not done): ");
        scanf("%d", &status);

        h->daysDone[day - 1] = status;
    }

    printf("Updated!\n");
}

// ===== SUMMARY (IMPROVED FORMAT) =====
void showSummary() {
    if (currentUser == -1) return;

    User *u = &users[currentUser];

    printf("\n========== %s ==========\n", u->name);

    printf("\n--- STUDY ---\n");
    for (int i = 0; i < u->studyCount; i++) {
        float pct = calculatePercentage(u->studyHabits[i]);
        int streak = calculateStreak(u->studyHabits[i]);

        printf("%-15s | %6.2f%% | Streak: %2d\n",
               u->studyHabits[i].name, pct, streak);

        showMotivation(pct, u->studyHabits[i].name);
    }

    printf("\n--- HOBBY ---\n");
    for (int i = 0; i < u->hobbyCount; i++) {
        float pct = calculatePercentage(u->hobbyHabits[i]);
        int streak = calculateStreak(u->hobbyHabits[i]);

        printf("%-15s | %6.2f%% | Streak: %2d\n",
               u->hobbyHabits[i].name, pct, streak);

        showMotivation(pct, u->hobbyHabits[i].name);
    }
}

// ===== VIEW SAVED DATA =====
void viewSavedData() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) return;

    fread(&userCount, sizeof(int), 1, fp);
    fread(users, sizeof(User), userCount, fp);
    fclose(fp);

    printf("\n===== SAVED DATA =====\n");

    for (int u = 0; u < userCount; u++) {
        printf("\nUser: %s\n", users[u].name);

        for (int i = 0; i < users[u].studyCount; i++)
            printf("%-15s | %6.2f%%\n",
                   users[u].studyHabits[i].name,
                   calculatePercentage(users[u].studyHabits[i]));

        for (int i = 0; i < users[u].hobbyCount; i++)
            printf("%-15s | %6.2f%%\n",
                   users[u].hobbyHabits[i].name,
                   calculatePercentage(users[u].hobbyHabits[i]));
    }
}

// ===== CALCULATIONS =====
float calculatePercentage(Habit h) {
    int c = 0;
    for (int i = 0; i < h.totalDays; i++)
        if (h.daysDone[i]) c++;

    if (h.totalDays == 0) return 0;
    return (c * 100.0f) / h.totalDays;
}

int calculateStreak(Habit h) {
    int max = 0, cur = 0;

    for (int i = 0; i < h.totalDays; i++) {
        if (h.daysDone[i]) {
            cur++;
            if (cur > max) max = cur;
        } else cur = 0;
    }
    return max;
}

// ===== MOTIVATION (IMPROVED TEXT) =====
void showMotivation(float pct, char name[]) {
    if (pct < 50)
        printf("  -> %s needs serious attention. Stay consistent.\n", name);
    else if (pct < 80)
        printf("  -> %s is improving. Keep pushing forward.\n", name);
    else
        printf("  -> %s is performing very well. Maintain this level.\n", name);
}

// ===== SAVE =====
void saveData() {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp) return;

    fwrite(&userCount, sizeof(int), 1, fp);
    fwrite(users, sizeof(User), userCount, fp);

    fclose(fp);
}

// ===== LOAD =====
void loadData() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) return;

    fread(&userCount, sizeof(int), 1, fp);
    fread(users, sizeof(User), userCount, fp);

    fclose(fp);
}
