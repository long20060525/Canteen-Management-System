//食堂菜品信息管理系统



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// 平台相关头文件
#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#define MAX_DISHES 100     // 最大菜品数量
#define MAX_FEEDBACKS 20   // 每个菜品最大反馈数量
#define MAX_INPUT 100      // 输入缓冲区大小
#define DATA_FILE "dishes.dat" // 数据文件名
#define ADMIN_PASSWORD "admin123" // 管理员密码

// 菜品反馈结构体
typedef struct {
    int score;             // 评分(1-5分)
    char comment[100];     // 评价内容
} Feedback;

// 菜品信息结构体
typedef struct {
    char window[50];       // 窗口名称
    char name[50];         // 菜品名称
    char ingredients[200]; // 食材用料
    char prepTime[20];     // 备菜时间
    char leftover[50];     // 剩菜处理方式
    Feedback feedbacks[MAX_FEEDBACKS]; // 学生反馈数组
    int feedbackCount;     // 反馈数量
    float avgScore;        // 平均评分
} Dish;

Dish dishes[MAX_DISHES];  // 菜品数组
int dishCount = 0;        // 当前菜品数量
bool isAdmin = false;     // 当前用户是否为管理员

// 函数声明
void loadData();
void saveData();
bool authenticateAdmin();
void disableEcho();
void enableEcho();
void showUserMenu();
void showAdminMenu();
void addDish();
void addFeedback();
void modifyDish();
void deleteDish();
void calculateAvgScores();
void sortDishes();
void searchDishes();
void displayAllDishes();
void displayDish(Dish dish);
void toLowerString(char *str);
bool isFileExists(const char *filename);
int findDishByName(const char *name);

// 主函数
int main() {
    int choice;
    
    // 启动时加载数据
    loadData();
    
    printf("=== 食堂菜品信息管理系统 ===\n");
    
    while(1) {
        printf("\n请选择用户类型:\n");
        printf("1. 普通用户\n");
        printf("2. 管理员登录\n");
        printf("0. 退出系统\n");
        printf("请选择: ");
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区中的换行符
        
        switch(choice) {
            case 1:
                isAdmin = false;
                showUserMenu();
                break;
            case 2:
                if(authenticateAdmin()) {
                    isAdmin = true;
                    showAdminMenu();
                } else {
                    printf("密码错误，权限不足！\n");
                }
                break;
            case 0:
                saveData(); // 退出前保存数据
                printf("数据已保存到 %s\n", DATA_FILE);
                printf("感谢使用食堂菜品信息管理系统，再见！\n");
                exit(0);
            default:
                printf("无效的选择，请重新输入！\n");
        }
    }
    
    return 0;
}

// 禁用终端回显(跨平台实现)
void disableEcho() {
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
#else
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
#endif
}

// 启用终端回显(跨平台实现)
void enableEcho() {
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode | ENABLE_ECHO_INPUT);
#else
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
#endif
}

// 管理员认证
bool authenticateAdmin() {
    char password[50];
    int i = 0;
    char ch;
    
    printf("请输入管理员密码: ");
    fflush(stdout);
    
    disableEcho(); // 禁用回显
    
    // 读取密码
    while((ch = getchar()) != '\n' && ch != EOF) {
        if(i < sizeof(password) - 1) {
            password[i++] = ch;
        }
    }
    password[i] = '\0';
    
    enableEcho(); // 重新启用回显
    printf("\n");
    
    return strcmp(password, ADMIN_PASSWORD) == 0;
}

// 普通用户菜单
void showUserMenu() {
    int choice;
    
    while(1) {
        printf("\n=== 普通用户菜单 ===\n");
        printf("1. 查询菜品\n");
        printf("2. 显示所有菜品信息\n");
        printf("3. 添加学生反馈\n");
        printf("0. 返回上级菜单\n");
        printf("请选择操作: ");
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区中的换行符
        
        switch(choice) {
            case 1:
                searchDishes();
                break;
            case 2:
                displayAllDishes();
                break;
            case 3:
                addFeedback();
                break;
            case 0:
                return; // 返回主菜单
            default:
                printf("无效的选择，请重新输入！\n");
        }
    }
}

// 管理员菜单
void showAdminMenu() {
    int choice;
    
    while(1) {
        printf("\n=== 管理员菜单 ===\n");
        printf("1. 添加菜品信息\n");
        printf("2. 修改菜品信息\n");
        printf("3. 删除菜品信息\n");
        printf("4. 计算平均评分\n");
        printf("5. 按评分排序菜品\n");
        printf("6. 查询菜品\n");
        printf("7. 显示所有菜品信息\n");
        printf("0. 返回上级菜单\n");
        printf("请选择操作: ");
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区中的换行符
        
        switch(choice) {
            case 1:
                addDish();
                break;
            case 2:
                modifyDish();
                break;
            case 3:
                deleteDish();
                break;
            case 4:
                calculateAvgScores();
                break;
            case 5:
                sortDishes();
                displayAllDishes();
                break;
            case 6:
                searchDishes();
                break;
            case 7:
                displayAllDishes();
                break;
            case 0:
                isAdmin = false; // 退出管理员模式
                return; // 返回主菜单
            default:
                printf("无效的选择，请重新输入！\n");
        }
    }
}

// 检查文件是否存在
bool isFileExists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// 从文件加载数据
void loadData() {
    if (!isFileExists(DATA_FILE)) {
        printf("数据文件不存在，将创建新文件。\n");
        return;
    }
    
    FILE *file = fopen(DATA_FILE, "rb");
    if (file == NULL) {
        printf("无法打开数据文件！\n");
        return;
    }
    
    // 读取菜品数量
    fread(&dishCount, sizeof(int), 1, file);
    
    // 读取菜品数据
    for (int i = 0; i < dishCount; i++) {
        fread(&dishes[i], sizeof(Dish), 1, file);
    }
    
    fclose(file);
    printf("已从 %s 加载 %d 条菜品数据\n", DATA_FILE, dishCount);
}

// 保存数据到文件
void saveData() {
    FILE *file = fopen(DATA_FILE, "wb");
    if (file == NULL) {
        printf("无法创建数据文件！\n");
        return;
    }
    
    // 写入菜品数量
    fwrite(&dishCount, sizeof(int), 1, file);
    
    // 写入菜品数据
    for (int i = 0; i < dishCount; i++) {
        fwrite(&dishes[i], sizeof(Dish), 1, file);
    }
    
    fclose(file);
}

// 根据菜品名称查找索引
int findDishByName(const char *name) {
    for(int i = 0; i < dishCount; i++) {
        if(strcmp(dishes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// 添加菜品信息
void addDish() {
    if(!isAdmin) {
        printf("权限不足，只有管理员可以添加菜品！\n");
        return;
    }
    
    if(dishCount >= MAX_DISHES) {
        printf("菜品数量已达上限，无法添加更多菜品！\n");
        return;
    }
    
    Dish newDish;
    newDish.feedbackCount = 0;
    newDish.avgScore = 0.0;
    
    printf("\n--- 添加新菜品 ---\n");
    
    printf("请输入窗口名称: ");
    fgets(newDish.window, sizeof(newDish.window), stdin);
    newDish.window[strcspn(newDish.window, "\n")] = '\0';
    
    printf("请输入菜品名称: ");
    fgets(newDish.name, sizeof(newDish.name), stdin);
    newDish.name[strcspn(newDish.name, "\n")] = '\0';
    
    // 检查菜品是否已存在
    if(findDishByName(newDish.name) != -1) {
        printf("该菜品已存在！\n");
        return;
    }
    
    printf("请输入食材用料(用逗号分隔): ");
    fgets(newDish.ingredients, sizeof(newDish.ingredients), stdin);
    newDish.ingredients[strcspn(newDish.ingredients, "\n")] = '\0';
    
    printf("请输入备菜时间(如: 30分钟): ");
    fgets(newDish.prepTime, sizeof(newDish.prepTime), stdin);
    newDish.prepTime[strcspn(newDish.prepTime, "\n")] = '\0';
    
    printf("请输入剩菜处理方式: ");
    fgets(newDish.leftover, sizeof(newDish.leftover), stdin);
    newDish.leftover[strcspn(newDish.leftover, "\n")] = '\0';
    
    dishes[dishCount++] = newDish;
    saveData(); // 添加后立即保存
    printf("菜品添加成功！\n");
}

// 添加学生反馈
void addFeedback() {
    if(dishCount == 0) {
        printf("当前没有菜品信息，请先添加菜品！\n");
        return;
    }
    
    char dishName[50];
    int dishIndex = -1;
    
    printf("\n--- 添加学生反馈 ---\n");
    printf("请输入要评价的菜品名称: ");
    fgets(dishName, sizeof(dishName), stdin);
    dishName[strcspn(dishName, "\n")] = '\0';
    
    // 查找菜品
    dishIndex = findDishByName(dishName);
    
    if(dishIndex == -1) {
        printf("未找到该菜品！\n");
        return;
    }
    
    if(dishes[dishIndex].feedbackCount >= MAX_FEEDBACKS) {
        printf("该菜品的反馈数量已达上限！\n");
        return;
    }
    
    Feedback newFeedback;
    
    printf("请输入评分(1-5分): ");
    scanf("%d", &newFeedback.score);
    getchar(); // 清除输入缓冲区中的换行符
    
    if(newFeedback.score < 1 || newFeedback.score > 5) {
        printf("评分必须在1-5分之间！\n");
        return;
    }
    
    printf("请输入评价内容: ");
    fgets(newFeedback.comment, sizeof(newFeedback.comment), stdin);
    newFeedback.comment[strcspn(newFeedback.comment, "\n")] = '\0';
    
    dishes[dishIndex].feedbacks[dishes[dishIndex].feedbackCount++] = newFeedback;
    // 实时计算新平均分
    float sum = 0;
    for(int i = 0; i < dishes[dishIndex].feedbackCount; i++) {
        sum += dishes[dishIndex].feedbacks[i].score;
    }
    dishes[dishIndex].avgScore = sum / dishes[dishIndex].feedbackCount;

    saveData();
    printf("反馈添加成功！当前评分：%.1f\n", dishes[dishIndex].avgScore);
    

}

// 修改菜品信息
void modifyDish() {
    if(!isAdmin) {
        printf("权限不足，只有管理员可以修改菜品信息！\n");
        return;
    }
    
    if(dishCount == 0) {
        printf("当前没有菜品信息！\n");
        return;
    }
    
    char dishName[50];
    int dishIndex = -1;
    
    printf("\n--- 修改菜品信息 ---\n");
    printf("请输入要修改的菜品名称: ");
    fgets(dishName, sizeof(dishName), stdin);
    dishName[strcspn(dishName, "\n")] = '\0';
    
    dishIndex = findDishByName(dishName);
    
    if(dishIndex == -1) {
        printf("未找到该菜品！\n");
        return;
    }
    
    printf("\n当前菜品信息:\n");
    displayDish(dishes[dishIndex]);
    
    printf("\n请输入新的窗口名称(原值: %s): ", dishes[dishIndex].window);
    fgets(dishes[dishIndex].window, sizeof(dishes[dishIndex].window), stdin);
    dishes[dishIndex].window[strcspn(dishes[dishIndex].window, "\n")] = '\0';
    
    printf("请输入新的食材用料(原值: %s): ", dishes[dishIndex].ingredients);
    fgets(dishes[dishIndex].ingredients, sizeof(dishes[dishIndex].ingredients), stdin);
    dishes[dishIndex].ingredients[strcspn(dishes[dishIndex].ingredients, "\n")] = '\0';
    
    printf("请输入新的备菜时间(原值: %s): ", dishes[dishIndex].prepTime);
    fgets(dishes[dishIndex].prepTime, sizeof(dishes[dishIndex].prepTime), stdin);
    dishes[dishIndex].prepTime[strcspn(dishes[dishIndex].prepTime, "\n")] = '\0';
    
    printf("请输入新的剩菜处理方式(原值: %s): ", dishes[dishIndex].leftover);
    fgets(dishes[dishIndex].leftover, sizeof(dishes[dishIndex].leftover), stdin);
    dishes[dishIndex].leftover[strcspn(dishes[dishIndex].leftover, "\n")] = '\0';
    
    saveData(); // 修改后立即保存
    printf("菜品信息修改成功！\n");
}

// 删除菜品信息
void deleteDish() {
    if(!isAdmin) {
        printf("权限不足，只有管理员可以删除菜品！\n");
        return;
    }
    
    if(dishCount == 0) {
        printf("当前没有菜品信息！\n");
        return;
    }
    
    char dishName[50];
    int dishIndex = -1;
    
    printf("\n--- 删除菜品信息 ---\n");
    printf("请输入要删除的菜品名称: ");
    fgets(dishName, sizeof(dishName), stdin);
    dishName[strcspn(dishName, "\n")] = '\0';
    
    dishIndex = findDishByName(dishName);
    
    if(dishIndex == -1) {
        printf("未找到该菜品！\n");
        return;
    }
    
    printf("\n将要删除的菜品信息:\n");
    displayDish(dishes[dishIndex]);
    
    printf("\n确认删除吗？(y/n): ");
    char confirm = getchar();
    getchar(); // 清除输入缓冲区中的换行符
    
    if(confirm == 'y' || confirm == 'Y') {
        // 将后面的元素前移
        for(int i = dishIndex; i < dishCount - 1; i++) {
            dishes[i] = dishes[i + 1];
        }
        dishCount--;
        
        saveData(); // 删除后立即保存
        printf("菜品删除成功！\n");
    } else {
        printf("取消删除操作。\n");
    }
}

// 计算所有菜品的平均评分
void calculateAvgScores() {
    if(!isAdmin) {
        printf("权限不足，只有管理员可以计算平均评分！\n");
        return;
    }
    
    if(dishCount == 0) {
        printf("当前没有菜品信息！\n");
        return;
    }
    
    for(int i = 0; i < dishCount; i++) {
        if(dishes[i].feedbackCount == 0) {
            dishes[i].avgScore = 0.0;
            continue;
        }
        
        int total = 0;
        for(int j = 0; j < dishes[i].feedbackCount; j++) {
            total += dishes[i].feedbacks[j].score;
        }
        dishes[i].avgScore = (float)total / dishes[i].feedbackCount;
    }
    
    saveData(); // 计算后保存
    printf("所有菜品的平均评分已计算完成！\n");
}

// 按平均分排序菜品(降序)
void sortDishes() {
    if(!isAdmin) {
        printf("权限不足，只有管理员可以排序菜品！\n");
        return;
    }
    
    if(dishCount == 0) {
        printf("当前没有菜品信息！\n");
        return;
    }
    
    // 使用简单的冒泡排序
    for(int i = 0; i < dishCount - 1; i++) {
        for(int j = 0; j < dishCount - i - 1; j++) {
            if(dishes[j].avgScore < dishes[j+1].avgScore) {
                Dish temp = dishes[j];
                dishes[j] = dishes[j+1];
                dishes[j+1] = temp;
            }
        }
    }
    
    printf("菜品已按平均评分排序！\n");
}

// 查询菜品(按名称或食材关键词)
void searchDishes() {
    if(dishCount == 0) {
        printf("当前没有菜品信息！\n");
        return;
    }
    
    char keyword[50];
    int found = 0;
    
    printf("\n--- 查询菜品 ---\n");
    printf("请输入菜品名称或食材关键词: ");
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = '\0';
    
    // 将关键词转换为小写便于比较
    toLowerString(keyword);
    
    printf("\n查询结果:\n");
    for(int i = 0; i < dishCount; i++) {
        char dishName[50];
        char ingredients[200];
        
        // 复制菜品名称和食材以便处理
        strcpy(dishName, dishes[i].name);
        strcpy(ingredients, dishes[i].ingredients);
        
        // 转换为小写
        toLowerString(dishName);
        toLowerString(ingredients);
        
        // 检查是否包含关键词
        if(strstr(dishName, keyword) || strstr(ingredients, keyword)) {
            displayDish(dishes[i]);
            found = 1;
        }
    }
    
    if(!found) {
        printf("未找到匹配的菜品！\n");
    }
}

// 显示所有菜品信息
void displayAllDishes() {
    if(dishCount == 0) {
        printf("当前没有菜品信息！\n");
        return;
    }
    
    printf("\n--- 所有菜品信息 (共%d条) ---\n", dishCount);
    for(int i = 0; i < dishCount; i++) {
        printf("\n菜品 #%d\n", i+1);
        displayDish(dishes[i]);
    }
}

// 显示单个菜品信息
void displayDish(Dish dish) {
    printf("窗口名称: %s\n", dish.window);
    printf("菜品名称: %s\n", dish.name);
    printf("食材用料: %s\n", dish.ingredients);
    printf("备菜时间: %s\n", dish.prepTime);
    printf("剩菜处理: %s\n", dish.leftover);
    printf("平均评分: %.1f\n", dish.avgScore);
    
    if(dish.feedbackCount > 0) {
        printf("学生反馈(%d条):\n", dish.feedbackCount);
        for(int i = 0; i < dish.feedbackCount; i++) {
            printf("  %d分: %s\n", dish.feedbacks[i].score, dish.feedbacks[i].comment);
        }
    } else {
        printf("暂无学生反馈\n");
    }
    printf("----------------------------\n");
}

// 将字符串转换为小写
void toLowerString(char *str) {
    for(int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}