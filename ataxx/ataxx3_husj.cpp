// 同化棋（Ataxx）简易交互样例程序
// 贪吃策略
// 作者：husj



#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <list>
#include <vector>
#include <string.h>

#include "jsoncpp/json.h"

using namespace std;

int currBotColor; // 我所执子颜色（1为黑，-1为白，棋盘状态亦同）
int gridInfo[7][7] = { 0 }; // 先x后y，记录棋盘状态
int blackPieceCount = 2, whitePieceCount = 2;
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } };

// 判断是否在地图内
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

// 向Direction方向改动坐标，并返回是否越界
inline bool MoveStep(int &x, int &y, int Direction)
{
	x = x + delta[Direction][0];
	y = y + delta[Direction][1];
	return inMap(x, y);
}

// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int color,int map[][7])
{
	if (color == 0)
		return false;
	if (x1 == -1) // 无路可走，跳过此回合
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // 超出边界
		return false;
	if (map[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	int effectivePoints[8][2];
	dx = abs((x0 - x1)), dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // 保证不会移动到原来位置，而且移动始终在5×5区域内
		return false;
	if (map[x1][y1] != 0) // 保证移动到的位置为空
		return false;
	if (dx == 2 || dy == 2) // 如果走的是5×5的外围，则不是复制粘贴
		map[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackPieceCount++;
		else
			whitePieceCount++;
	}

	map[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // 影响邻近8个位置
	{
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inMap(x, y))
			continue;
		if (map[x][y] == -color)
		{
			effectivePoints[currCount][0] = x;
			effectivePoints[currCount][1] = y;
			currCount++;
			map[x][y] = color;
		}
	}
	if (currCount != 0)
	{
		if (color == 1)
		{
			blackPieceCount += currCount;
			whitePieceCount -= currCount;
		}
		else
		{
			whitePieceCount += currCount;
			blackPieceCount -= currCount;
		}
	}
	return true;
}


// 给定棋盘和颜色，取出该颜色可以下的所有位置
vector<pair<pair<int,int>, pair<int,int> > > getAllPossibel(int color, int map[][7]) {

    vector<pair<pair<int,int>,pair<int,int> > >  allPossible;

    for (int y0 = 0; y0 < 7; y0++)
        for (int x0 = 0; x0 < 7; x0++)
        {
            // 该位置已经有子，但是颜色不一样或者还空着
            if (map[x0][y0] != color)
                continue;

            //  该位置有我方棋子，尝试上下左右的24个可能性
            for (int dir = 0; dir < 24; dir++)
            {
                int x1 = x0 + delta[dir][0];
                int y1 = y0 + delta[dir][1];
                if (!inMap(x1, y1))
                    continue;
                if (map[x1][y1] != 0)
                    continue;

                // 找到一个位置
                pair<int, int> beginPos(x0,y0);
                pair<int, int> possiblePos(x1,y1);

                allPossible.push_back(make_pair(beginPos,possiblePos));
            }
        }

    return allPossible;
}

// 取出黑棋和白起棋的个数
pair<int, int> getPieceCount(int map[][7]){
    int blackPieceCount = 0;
    int whitePieceCount = 0;

    for (int i = 0; i < 7; i++)
        for (int j = 0; j < 7; j++)
        {
                if(map[i][j]==1)
                    blackPieceCount++;
                else if(map[i][j]==-1)
                    whitePieceCount++;
        }

    return make_pair(blackPieceCount, whitePieceCount);
}

// 给定棋盘，位置，颜色，在棋盘上走一步
void one_step(pair<pair<int,int>, pair<int,int> > pos, int color, int map[][7]){
    pair<int,int> beginPos= pos.first;
    pair<int,int> possiblePos = pos.second;

    // 取出下棋的位置
    int startX, startY, resultX, resultY;
    startX = beginPos.first;
    startY = beginPos.second;
    resultX = possiblePos.first;
    resultY = possiblePos.second;
    // 走一步
    ProcStep(startX, startY, resultX, resultY, color, map);
}


// 胡时京的贪吃对策。给定棋盘，颜色和可能下的位置，选择一个最优位置
// 贪吃对策：
//     假定当前我有m种下法，走一步后，对方有n种下法。
//     我遍历m*n种下法，取出每种下法我的棋子的个数，
//     棋子的个数最大的下法，为我的最优解
pair<pair<int,int>, pair<int,int> > husjSelect(vector<pair<pair<int,int>, pair<int,int> > > &allPossible, int color, int map[][7]){

    int myMap[7][7] = { 0 };
    int otherMap[7][7] = { 0 };
    int thirMap[7][7] = { 0 };

    // 最优位置
    int goodCnt = -100;
    pair<pair<int,int>, pair<int,int> > goodPossible;

    // 我的下棋位子
    pair<pair<int,int>, pair<int,int> > posMy;
    // 模拟的机器的下棋位子
    pair<pair<int,int>, pair<int,int> > posOther;
    // 模拟的my的下棋位子
    pair<pair<int,int>, pair<int,int> > posThir;

    // 取出黑白棋的个数
    pair<int,int> pieceCount_bef = getPieceCount(map);

    // 遍历我的下棋位置可能性
    for (int i = 0 ; i < (int)allPossible.size() ; i++){
        // 先备份棋盘状态，不要把实际的棋盘破坏了
        memcpy(myMap, map, sizeof(myMap));

        // 取出一个位置
        posMy = allPossible[i];
        // 在我的棋盘上，模拟走一步
        one_step(posMy, color, myMap);

        // 取出对方可能下的位置
        vector<pair<pair<int,int>, pair<int,int> > > allPossibleOther =  getAllPossibel(-color, myMap);
        if (allPossibleOther.size() == 0){
        	return posMy;
        }
        for (int j = 0 ; j < (int)allPossibleOther.size() ; j++){
            // 先备份棋盘状态，不要把我的棋盘破坏了
            memcpy(otherMap, myMap, sizeof(otherMap));

            // 取出对方可能下的一个位置
            posOther = allPossibleOther[j];
            // 模拟走一步
            one_step(posOther, -color, otherMap);

            // 取出对方可能下的位置
             vector<pair<pair<int,int>, pair<int,int> > > allPossibleThir =  getAllPossibel(color, otherMap);
             if (allPossibleThir.size() == 0){
            	 break;
             }
             for (int m = 0 ; m < (int)allPossibleThir.size() ; m++){
                 // 先备份棋盘状态，不要把我的棋盘破坏了
                 memcpy(thirMap, otherMap, sizeof(thirMap));

                 // 取出对方可能下的一个位置
                 posThir = allPossibleThir[m];
                 // 模拟走一步
                 one_step(posThir, color, thirMap);


				// 取出黑白棋的个数
				pair<int,int> pieceCount = getPieceCount(thirMap);

				// 我是黑棋的情况下，选择黑棋最多的下法
				int cnt;
				if (color == 1)
					//cnt = pieceCount.first-pieceCount.second;
					cnt = pieceCount_bef.second-pieceCount.second;
				else
					// 我是白棋的情况下，选择白棋最多的下法
					//cnt = pieceCount.second-pieceCount.first;
					cnt = pieceCount_bef.first-pieceCount.first;

				// 备份当前最好的下法
				if (cnt >= goodCnt){
					goodCnt = cnt;
					goodPossible = posMy;
				}
             }
        }
    }
    return goodPossible;
}

string dbg="";
pair<pair<int,int>, pair<int,int> > husjSelect_d1(vector<pair<pair<int,int>, pair<int,int> > > &allPossible, int color, int map[][7]){

    int myMap[7][7] = { 0 };

    // 最优位置
    int goodCnt = -100;
    pair<pair<int,int>, pair<int,int> > goodPossible;

    // 我的下棋位子
    pair<pair<int,int>, pair<int,int> > posMy;


    // 遍历我的下棋位置可能性
    // 取出黑白棋的个数
    pair<int,int> pieceCount_bef = getPieceCount(map);
    for (int i = 0 ; i < (int)allPossible.size() ; i++){
        // 先备份棋盘状态，不要把实际的棋盘破坏了
        //memcpy(myMap, map, sizeof(myMap));

    	for (int m=0 ; m<7; ++m)
    		for(int n=0 ; n<7 ;++n)
    			myMap[m][n]= map[m][n];

        // 取出一个位置
        posMy = allPossible[i];
        // 在我的棋盘上，模拟走一步

		one_step(posMy, color, myMap);

		// 取出黑白棋的个数
		pair<int,int> pieceCount = getPieceCount(myMap);

		// 我是黑棋的情况下，选择黑棋最多的下法
		int cnt;
		if (color == 1)
			//cnt = pieceCount.first-pieceCount.second;
			cnt = pieceCount_bef.second-pieceCount.second;
		else
			// 我是白棋的情况下，选择白棋最多的下法
			//cnt = pieceCount.second-pieceCount.first;
			cnt = pieceCount_bef.first-pieceCount.first;

		// 备份当前最好的下法
		if (cnt >= goodCnt){
			goodCnt = cnt;
			goodPossible = posMy;
		}

    }


    return goodPossible;
}



int main()
{
	istream::sync_with_stdio(false);

	int x0, y0, x1, y1;

	// 初始化棋盘
	gridInfo[0][0] = gridInfo[6][6] = 1;  //|黑|白|
	gridInfo[6][0] = gridInfo[0][6] = -1; //|白|黑|

	// 读入JSON
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);



// 分析自己收到的输入和自己过往的输出，并恢复状态
	int turnID = input["responses"].size();
	currBotColor = input["requests"][(Json::Value::UInt) 0]["x0"].asInt() < 0 ? 1 : -1; // 第一回合收到坐标是-1, -1，说明我是黑方
	for (int i = 0; i < turnID; i++)
	{
		// 根据这些输入输出逐渐恢复状态到当前回合
		x0 = input["requests"][i]["x0"].asInt();
		y0 = input["requests"][i]["y0"].asInt();
		x1 = input["requests"][i]["x1"].asInt();
		y1 = input["requests"][i]["y1"].asInt();
		if (x1 >= 0)
			ProcStep(x0, y0, x1, y1, -currBotColor,gridInfo); // 模拟对方落子
		x0 = input["responses"][i]["x0"].asInt();
		y0 = input["responses"][i]["y0"].asInt();
		x1 = input["responses"][i]["x1"].asInt();
		y1 = input["responses"][i]["y1"].asInt();
		if (x1 >= 0)
			ProcStep(x0, y0, x1, y1, currBotColor,gridInfo); // 模拟己方落子
	}

	// 看看自己本回合输入
	x0 = input["requests"][turnID]["x0"].asInt();
	y0 = input["requests"][turnID]["y0"].asInt();
	x1 = input["requests"][turnID]["x1"].asInt();
	y1 = input["requests"][turnID]["y1"].asInt();
	if (x1 >= 0)
		ProcStep(x0, y0, x1, y1, -currBotColor,gridInfo); // 模拟对方落子

	vector<pair<pair<int,int>,pair<int,int> > >  allPossible;
	// 找出合法落子点
	allPossible =  getAllPossibel(currBotColor, gridInfo);
	int posCount = allPossible.size();

	// 做出决策（你只需修改以下部分）
	int startX, startY, resultX, resultY;

	pair<pair<int,int>, pair<int,int> > pos;
	if (posCount > 0)
	{
		// 胡时京的贪吃算法找到一个最优解位置
		pos = husjSelect(allPossible, currBotColor, gridInfo);
		startX = pos.first.first;
		startY = pos.first.second;
		resultX = pos.second.first;
		resultY = pos.second.second;
	}
	else
	{
		startX = -1;
		startY = -1;
		resultX = -1;
		resultY = -1;
	}

	// 决策结束，输出结果（你只需修改以上部分）

	Json::Value ret;
	ret["response"]["x0"] = startX;
	ret["response"]["y0"] = startY;
	ret["response"]["x1"] = resultX;
	ret["response"]["y1"] = resultY;
	//ret["response"]["debug"] = dbg;
	Json::FastWriter writer;
	cout << writer.write(ret) << endl;

	return 0;
}
