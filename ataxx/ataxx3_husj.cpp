// ͬ���壨Ataxx�����׽�����������
// ̰�Բ���
// ���ߣ�husj



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

int currBotColor; // ����ִ����ɫ��1Ϊ�ڣ�-1Ϊ�ף�����״̬��ͬ��
int gridInfo[7][7] = { 0 }; // ��x��y����¼����״̬
int blackPieceCount = 2, whitePieceCount = 2;
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } };

// �ж��Ƿ��ڵ�ͼ��
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

// ��Direction����Ķ����꣬�������Ƿ�Խ��
inline bool MoveStep(int &x, int &y, int Direction)
{
	x = x + delta[Direction][0];
	y = y + delta[Direction][1];
	return inMap(x, y);
}

// �����괦���ӣ�����Ƿ�Ϸ���ģ������
bool ProcStep(int x0, int y0, int x1, int y1, int color,int map[][7])
{
	if (color == 0)
		return false;
	if (x1 == -1) // ��·���ߣ������˻غ�
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // �����߽�
		return false;
	if (map[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	int effectivePoints[8][2];
	dx = abs((x0 - x1)), dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // ��֤�����ƶ���ԭ��λ�ã������ƶ�ʼ����5��5������
		return false;
	if (map[x1][y1] != 0) // ��֤�ƶ�����λ��Ϊ��
		return false;
	if (dx == 2 || dy == 2) // ����ߵ���5��5����Χ�����Ǹ���ճ��
		map[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackPieceCount++;
		else
			whitePieceCount++;
	}

	map[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // Ӱ���ڽ�8��λ��
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


// �������̺���ɫ��ȡ������ɫ�����µ�����λ��
vector<pair<pair<int,int>, pair<int,int> > > getAllPossibel(int color, int map[][7]) {

    vector<pair<pair<int,int>,pair<int,int> > >  allPossible;

    for (int y0 = 0; y0 < 7; y0++)
        for (int x0 = 0; x0 < 7; x0++)
        {
            // ��λ���Ѿ����ӣ�������ɫ��һ�����߻�����
            if (map[x0][y0] != color)
                continue;

            //  ��λ�����ҷ����ӣ������������ҵ�24��������
            for (int dir = 0; dir < 24; dir++)
            {
                int x1 = x0 + delta[dir][0];
                int y1 = y0 + delta[dir][1];
                if (!inMap(x1, y1))
                    continue;
                if (map[x1][y1] != 0)
                    continue;

                // �ҵ�һ��λ��
                pair<int, int> beginPos(x0,y0);
                pair<int, int> possiblePos(x1,y1);

                allPossible.push_back(make_pair(beginPos,possiblePos));
            }
        }

    return allPossible;
}

// ȡ������Ͱ�����ĸ���
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

// �������̣�λ�ã���ɫ������������һ��
void one_step(pair<pair<int,int>, pair<int,int> > pos, int color, int map[][7]){
    pair<int,int> beginPos= pos.first;
    pair<int,int> possiblePos = pos.second;

    // ȡ�������λ��
    int startX, startY, resultX, resultY;
    startX = beginPos.first;
    startY = beginPos.second;
    resultX = possiblePos.first;
    resultY = possiblePos.second;
    // ��һ��
    ProcStep(startX, startY, resultX, resultY, color, map);
}


// ��ʱ����̰�ԶԲߡ��������̣���ɫ�Ϳ����µ�λ�ã�ѡ��һ������λ��
// ̰�ԶԲߣ�
//     �ٶ���ǰ����m���·�����һ���󣬶Է���n���·���
//     �ұ���m*n���·���ȡ��ÿ���·��ҵ����ӵĸ�����
//     ���ӵĸ��������·���Ϊ�ҵ����Ž�
pair<pair<int,int>, pair<int,int> > husjSelect(vector<pair<pair<int,int>, pair<int,int> > > &allPossible, int color, int map[][7]){

    int myMap[7][7] = { 0 };
    int otherMap[7][7] = { 0 };
    int thirMap[7][7] = { 0 };

    // ����λ��
    int goodCnt = -100;
    pair<pair<int,int>, pair<int,int> > goodPossible;

    // �ҵ�����λ��
    pair<pair<int,int>, pair<int,int> > posMy;
    // ģ��Ļ���������λ��
    pair<pair<int,int>, pair<int,int> > posOther;
    // ģ���my������λ��
    pair<pair<int,int>, pair<int,int> > posThir;

    // ȡ���ڰ���ĸ���
    pair<int,int> pieceCount_bef = getPieceCount(map);

    // �����ҵ�����λ�ÿ�����
    for (int i = 0 ; i < (int)allPossible.size() ; i++){
        // �ȱ�������״̬����Ҫ��ʵ�ʵ������ƻ���
        memcpy(myMap, map, sizeof(myMap));

        // ȡ��һ��λ��
        posMy = allPossible[i];
        // ���ҵ������ϣ�ģ����һ��
        one_step(posMy, color, myMap);

        // ȡ���Է������µ�λ��
        vector<pair<pair<int,int>, pair<int,int> > > allPossibleOther =  getAllPossibel(-color, myMap);
        if (allPossibleOther.size() == 0){
        	return posMy;
        }
        for (int j = 0 ; j < (int)allPossibleOther.size() ; j++){
            // �ȱ�������״̬����Ҫ���ҵ������ƻ���
            memcpy(otherMap, myMap, sizeof(otherMap));

            // ȡ���Է������µ�һ��λ��
            posOther = allPossibleOther[j];
            // ģ����һ��
            one_step(posOther, -color, otherMap);

            // ȡ���Է������µ�λ��
             vector<pair<pair<int,int>, pair<int,int> > > allPossibleThir =  getAllPossibel(color, otherMap);
             if (allPossibleThir.size() == 0){
            	 break;
             }
             for (int m = 0 ; m < (int)allPossibleThir.size() ; m++){
                 // �ȱ�������״̬����Ҫ���ҵ������ƻ���
                 memcpy(thirMap, otherMap, sizeof(thirMap));

                 // ȡ���Է������µ�һ��λ��
                 posThir = allPossibleThir[m];
                 // ģ����һ��
                 one_step(posThir, color, thirMap);


				// ȡ���ڰ���ĸ���
				pair<int,int> pieceCount = getPieceCount(thirMap);

				// ���Ǻ��������£�ѡ����������·�
				int cnt;
				if (color == 1)
					//cnt = pieceCount.first-pieceCount.second;
					cnt = pieceCount_bef.second-pieceCount.second;
				else
					// ���ǰ��������£�ѡ����������·�
					//cnt = pieceCount.second-pieceCount.first;
					cnt = pieceCount_bef.first-pieceCount.first;

				// ���ݵ�ǰ��õ��·�
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

    // ����λ��
    int goodCnt = -100;
    pair<pair<int,int>, pair<int,int> > goodPossible;

    // �ҵ�����λ��
    pair<pair<int,int>, pair<int,int> > posMy;


    // �����ҵ�����λ�ÿ�����
    // ȡ���ڰ���ĸ���
    pair<int,int> pieceCount_bef = getPieceCount(map);
    for (int i = 0 ; i < (int)allPossible.size() ; i++){
        // �ȱ�������״̬����Ҫ��ʵ�ʵ������ƻ���
        //memcpy(myMap, map, sizeof(myMap));

    	for (int m=0 ; m<7; ++m)
    		for(int n=0 ; n<7 ;++n)
    			myMap[m][n]= map[m][n];

        // ȡ��һ��λ��
        posMy = allPossible[i];
        // ���ҵ������ϣ�ģ����һ��

		one_step(posMy, color, myMap);

		// ȡ���ڰ���ĸ���
		pair<int,int> pieceCount = getPieceCount(myMap);

		// ���Ǻ��������£�ѡ����������·�
		int cnt;
		if (color == 1)
			//cnt = pieceCount.first-pieceCount.second;
			cnt = pieceCount_bef.second-pieceCount.second;
		else
			// ���ǰ��������£�ѡ����������·�
			//cnt = pieceCount.second-pieceCount.first;
			cnt = pieceCount_bef.first-pieceCount.first;

		// ���ݵ�ǰ��õ��·�
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

	// ��ʼ������
	gridInfo[0][0] = gridInfo[6][6] = 1;  //|��|��|
	gridInfo[6][0] = gridInfo[0][6] = -1; //|��|��|

	// ����JSON
	string str;
	getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);



// �����Լ��յ���������Լ���������������ָ�״̬
	int turnID = input["responses"].size();
	currBotColor = input["requests"][(Json::Value::UInt) 0]["x0"].asInt() < 0 ? 1 : -1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�
	for (int i = 0; i < turnID; i++)
	{
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�
		x0 = input["requests"][i]["x0"].asInt();
		y0 = input["requests"][i]["y0"].asInt();
		x1 = input["requests"][i]["x1"].asInt();
		y1 = input["requests"][i]["y1"].asInt();
		if (x1 >= 0)
			ProcStep(x0, y0, x1, y1, -currBotColor,gridInfo); // ģ��Է�����
		x0 = input["responses"][i]["x0"].asInt();
		y0 = input["responses"][i]["y0"].asInt();
		x1 = input["responses"][i]["x1"].asInt();
		y1 = input["responses"][i]["y1"].asInt();
		if (x1 >= 0)
			ProcStep(x0, y0, x1, y1, currBotColor,gridInfo); // ģ�⼺������
	}

	// �����Լ����غ�����
	x0 = input["requests"][turnID]["x0"].asInt();
	y0 = input["requests"][turnID]["y0"].asInt();
	x1 = input["requests"][turnID]["x1"].asInt();
	y1 = input["requests"][turnID]["y1"].asInt();
	if (x1 >= 0)
		ProcStep(x0, y0, x1, y1, -currBotColor,gridInfo); // ģ��Է�����

	vector<pair<pair<int,int>,pair<int,int> > >  allPossible;
	// �ҳ��Ϸ����ӵ�
	allPossible =  getAllPossibel(currBotColor, gridInfo);
	int posCount = allPossible.size();

	// �������ߣ���ֻ���޸����²��֣�
	int startX, startY, resultX, resultY;

	pair<pair<int,int>, pair<int,int> > pos;
	if (posCount > 0)
	{
		// ��ʱ����̰���㷨�ҵ�һ�����Ž�λ��
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

	// ���߽���������������ֻ���޸����ϲ��֣�

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
