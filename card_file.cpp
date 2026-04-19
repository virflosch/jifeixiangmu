#include "card_file.h"
#include <fstream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdio>
#include "card_service.h"

namespace
{
    int appendCardToList(const Card &card)
    {
        lpCardNode newNode = new CardNode;
        if (newNode == nullptr)
        {
            return -1;
        }

        newNode->data = card;
        newNode->next = nullptr;

        lpCardNode current = cardList;
        while (current->next != nullptr)
        {
            current = current->next;
        }
        current->next = newNode;
        return 0;
    }

    int writeCardsToBinaryFile()
    {
        std::ofstream file(CARD_FILE_PATH, std::ios::binary | std::ios::trunc);
        if (!file)
        {
            return -1;
        }

        if (cardList != nullptr)
        {
            lpCardNode current = cardList->next;
            while (current != nullptr)
            {
                file.write(reinterpret_cast<const char *>(&current->data), sizeof(Card));
                if (!file)
                {
                    return -2;
                }
                current = current->next;
            }
        }

        file.close();
        return 0;
    }

    int loadCardsFromBinaryFile()
    {
        std::ifstream file(CARD_FILE_PATH, std::ios::binary);
        if (!file)
        {
            return 0;
        }

        int count = 0;
        Card card;
        while (file.read(reinterpret_cast<char *>(&card), sizeof(Card)))
        {
            if (appendCardToList(card) != 0)
            {
                file.close();
                return -1;
            }
            count++;
        }

        file.close();
        return count;
    }

    int loadCardsFromTextFile()
    {
        std::ifstream file(CARD_TEXT_FILE_PATH);
        if (!file)
        {
            return 0;
        }

        std::string line;
        int count = 0;
        while (std::getline(file, line))
        {
            if (line.empty() || line.find("##") == std::string::npos)
            {
                continue;
            }

            char *buf = new char[line.size() + 1];
            strcpy(buf, line.c_str());
            Card card = parseCard(buf);
            delete[] buf;

            if (strlen(card.aName) == 0)
            {
                continue;
            }

            if (appendCardToList(card) != 0)
            {
                file.close();
                return -1;
            }
            count++;
        }

        file.close();

        if (count > 0)
        {
            writeCardsToBinaryFile();
        }

        return count;
    }
}

// 保存卡信息到文件
int saveCards(Card cards[], int count)
{
    std::ofstream file(CARD_FILE_PATH, std::ios::binary | std::ios::trunc);
    if (!file)
    {
        return -1;
    }
    file.write(reinterpret_cast<char *>(cards), sizeof(Card) * count);
    file.close();
    return 0;
}

// 从文件加载卡信息
int loadCards(Card cards[])
{
    std::ifstream file(CARD_FILE_PATH, std::ios::binary);
    if (!file)
    {
        return 0;
    }
    file.seekg(0, std::ios::end);
    int count = file.tellg() / sizeof(Card);
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(cards), sizeof(Card) * count);
    file.close();
    return count;
}

// 将时间转换为字符串
std::string timeToString(time_t t)
{
    std::tm *tm_info = std::localtime(&t);
    std::stringstream ss;
    ss << std::setw(4) << std::setfill('0') << (tm_info->tm_year + 1900) << "-";
    ss << std::setw(2) << std::setfill('0') << (tm_info->tm_mon + 1) << "-";
    ss << std::setw(2) << std::setfill('0') << tm_info->tm_mday << " ";
    ss << std::setw(2) << std::setfill('0') << tm_info->tm_hour << ":";
    ss << std::setw(2) << std::setfill('0') << tm_info->tm_min;
    return ss.str();
}

// 将字符串转换为时间
time_t stringToTime(char *pTime)
{
    int year, month, day, hour, minute;
    sscanf(pTime, "%d-%d-%d %d:%d", &year, &month, &day, &hour, &minute);

    std::tm tm_info = {0};
    tm_info.tm_year = year - 1900;
    tm_info.tm_mon = month - 1;
    tm_info.tm_mday = day;
    tm_info.tm_hour = hour;
    tm_info.tm_min = minute;

    return mktime(&tm_info);
}

// 将字符串解析为Card结构体
Card parseCard(char *pBuf)
{
    Card card;
    memset(&card, 0, sizeof(Card));

    if (pBuf == nullptr || strlen(pBuf) == 0)
    {
        return card;
    }

    char *token = strtok(pBuf, "##");

    if (token)
    {
        strncpy(card.aName, token, sizeof(card.aName) - 1);
        card.aName[sizeof(card.aName) - 1] = '\0';
    }
    token = strtok(nullptr, "##");
    if (token)
    {
        strncpy(card.aPwd, token, sizeof(card.aPwd) - 1);
        card.aPwd[sizeof(card.aPwd) - 1] = '\0';
    }
    token = strtok(nullptr, "##");
    if (token)
        card.nStatus = std::stoi(token);
    token = strtok(nullptr, "##");
    if (token)
        card.tStart = stringToTime(token);
    token = strtok(nullptr, "##");
    if (token)
        card.tEnd = stringToTime(token);
    token = strtok(nullptr, "##");
    if (token)
        card.fTotalUse = std::stof(token);
    token = strtok(nullptr, "##");
    if (token)
        card.tLast = stringToTime(token);
    token = strtok(nullptr, "##");
    if (token)
        card.nUseCount = std::stoi(token);
    token = strtok(nullptr, "##");
    if (token)
        card.fBalance = std::stof(token);
    token = strtok(nullptr, "##");
    if (token)
        card.nDel = std::stoi(token);

    return card;
}

// 保存单条卡信息到二进制文件
int saveCard(const Card *pCard, const char *pPath)
{
    (void)pPath;

    std::ofstream file(CARD_FILE_PATH, std::ios::binary | std::ios::app);
    if (!file)
    {
        return -1;
    }

    file.write(reinterpret_cast<const char *>(pCard), sizeof(Card));
    if (!file)
    {
        return -2;
    }

    file.close();
    return 0;
}

// 获取文件中卡信息的条数
int getCardCount(const char *pPath)
{
    std::ifstream file(pPath);
    if (!file)
    {
        return 0;
    }

    int count = 0;
    std::string line;
    while (std::getline(file, line))
    {
        count++;
    }

    file.close();
    return count;
}

// 从文件中读取一条卡信息
int readCard(Card *pCard, const char *pPath)
{
    std::ifstream file(pPath);
    if (!file)
    {
        return -1;
    }

    std::string line;
    if (std::getline(file, line))
    {
        char *buf = new char[line.size() + 1];
        strcpy(buf, line.c_str());
        *pCard = parseCard(buf);
        delete[] buf;
        file.close();
        return 0;
    }

    file.close();
    return -1;
}

// 从文件读取所有卡信息到链表
int getCard()
{
    releaseCardList();
    initCardList();

    int count = loadCardsFromBinaryFile();
    if (count > 0)
    {
        return count;
    }

    int textCount = loadCardsFromTextFile();
    if (textCount > 0)
    {
        return textCount;
    }

    return count;
}

// 更新文件中的卡信息
int updateCard(const Card *card, const char *pPath)
{
    (void)pPath;

    if (card == nullptr || cardList == nullptr)
    {
        return -1;
    }

    lpCardNode current = cardList->next;
    while (current != nullptr)
    {
        if (strcmp(current->data.aName, card->aName) == 0)
        {
            Card oldCard = current->data;
            current->data = *card;

            if (writeCardsToBinaryFile() != 0)
            {
                current->data = oldCard;
                return -2;
            }

            return 0;
        }

        current = current->next;
    }

    return -1;
}
