// 同化棋（Ataxx）对战程序
// 胡时京策略
// 作者：husj

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <list>
#include <vector>
#include <string.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include<algorithm>
using namespace std;

// 执子颜色（1为黑，-1为白，棋盘状态亦同）
// 我的颜色是黑色
int myColor = 1;
// 机器的颜色是白色
int otherColor = -1;

// 棋盘状态
int gridInfo[7][7] = { 0 }; // 先x后y，记录棋盘状态

// 走棋的历史记录，用于复盘
list<vector<int> > history;
bool saveFlg; // 存盘否标志

// 24个可能放置棋子的位置定义
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

// 在坐标处落子，检查是否合法或模拟落子
bool ProcStep(int x0, int y0, int x1, int y1, int color, int map[][7])
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

    dx = abs((x0 - x1)), dy = abs((y0 - y1));
    if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // 保证不会移动到原来位置，而且移动始终在5×5区域内
        return false;
    if (map[x1][y1] != 0) // 保证移动到的位置为空
        return false;
    if (dx == 2 || dy == 2) // 如果走的是5×5的外围，则不是复制粘贴
        map[x0][y0] = 0;

    map[x1][y1] = color;
    for (dir = 0; dir < 8; dir++) // 影响邻近8个位置
    {
        x = x1 + delta[dir][0];
        y = y1 + delta[dir][1];
        if (!inMap(x, y))
            continue;
        if (map[x][y] == -color)
        {
            currCount++;
            map[x][y] = color;
        }
    }

    return true;
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

// 机器的随机对策选择下棋位置
pair<pair<int,int>, pair<int,int> > randomSelect(vector<pair<pair<int,int>, pair<int,int> > > &allPossible){

    // 取出有几种可能性
    int posCount = allPossible.size();

    // 产生一个随机数
    srand(time(0));
    int choice = rand() % posCount;

    // 随机选一个位置
    pair<int, int> beginPos = allPossible[choice].first;
    pair<int, int> possiblePos = allPossible[choice].second;

    return make_pair(beginPos, possiblePos);
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
//     当对方选取对方最优下法时，我有m2种下法。
//     遍历这m2种下法后找到我当前最优的下法。
//     比较之前m种下法中当前最优的下法，找到我在这m种下法中最优的下法。
pair<pair<int,int>, pair<int,int> > husjSelect(vector<pair<pair<int,int>, pair<int,int> > > &allPossible, int color, int map[][7]){

    int myMap[7][7] = { 0 };
    int otherMap[7][7] = { 0 };

    int goodCnt = -100;
    int badCnt = 100;

    // 最优位置
	pair<pair<int,int>, pair<int,int> > goodPossible;

	// 对方最优位置
	pair<pair<int,int>, pair<int,int> > badPossible;

    // 我的下棋位子
    pair<pair<int,int>, pair<int,int> > posMy;
    // 模拟的机器的下棋位子
    pair<pair<int,int>, pair<int,int> > posOther;
    // 我的下棋位子2
    pair<pair<int,int>, pair<int,int> > posMy2;

    // 遍历我的下棋位置可能性
    // 取出黑白棋的个数
    pair<int,int> pieceCount_bef = getPieceCount(map);

    for (int i = 0 ; i < (int)allPossible.size() ; i++){
        // 先备份棋盘状态，不要把实际的棋盘破坏了
        memcpy(myMap, map, sizeof(myMap));

        // 取出一个位置
        posMy = allPossible[i];
        // 在我的棋盘上，模拟走一步
        one_step(posMy, color, myMap);

        badCnt=100;

        // 取出对方可能下的位置
        vector<pair<pair<int,int>, pair<int,int> > > allPossibleOther =  getAllPossibel(-color, myMap);
        for (int j = 0 ; j < (int)allPossibleOther.size() ; j++){
            // 先备份棋盘状态，不要把我的棋盘破坏了
            memcpy(otherMap, myMap, sizeof(otherMap));

            // 取出对方可能下的一个位置
            posOther = allPossibleOther[j];
            // 模拟走一步
            one_step(posOther, -color, otherMap);

            // 取出黑白棋的个数
            pair<int,int> pieceCount = getPieceCount(otherMap);

            // 我是黑棋的情况下，选择黑棋最多的下法
            int cnt;
            if (color == 1)
				cnt = pieceCount.first-pieceCount.second;
				//cnt = pieceCount_bef.second-pieceCount.second;
			else
				// 我是白棋的情况下，选择白棋最多的下法
				cnt = pieceCount.second-pieceCount.first;
				//cnt = pieceCount_bef.first-pieceCount.first;

			// 备份当前对方最好的下法
			if (cnt <= badCnt){
				badCnt = cnt;
				badPossible = posOther;
			}
        }
        //对方按最优下法走一步
        one_step(badPossible, -color,  myMap);

        // 取出我可能下的位置
        vector<pair<pair<int,int>, pair<int,int> > > allPossibleOther2 =  getAllPossibel(color, myMap);
        for (int j = 0 ; j < (int)allPossibleOther2.size() ; j++){
            // 先备份棋盘状态，不要把我的棋盘破坏了
            memcpy(otherMap, myMap, sizeof(otherMap));

            // 取出我可能下的一个位置
            posMy2 = allPossibleOther2[j];
            // 模拟走一步
            one_step(posMy2, color, otherMap);

            // 取出黑白棋的个数
            pair<int,int> pieceCount = getPieceCount(otherMap);

            // 我是黑棋的情况下，选择黑棋最多的下法
            int cnt;
            if (color == 1)
				cnt = pieceCount.first-pieceCount.second;
				//cnt = pieceCount_bef.second-pieceCount.second;
			else
				// 我是白棋的情况下，选择白棋最多的下法
				cnt = pieceCount.second-pieceCount.first;
				//cnt = pieceCount_bef.first-pieceCount.first;

			// 备份我最好的下法
			if (cnt >= goodCnt){
				goodCnt = cnt;
				goodPossible = posMy;
			}
        }

    }
    return goodPossible;
}

pair<pair<int,int>, pair<int,int> > husjSelect_d1(vector<pair<pair<int,int>, pair<int,int> > > &allPossible, int color, int map[][7]){

    int myMap[7][7] = { 0 };


    // 最优位置
    int goodCnt = -100;
    pair<pair<int,int>, pair<int,int> > goodPossible;

    // 我的下棋位子
    pair<pair<int,int>, pair<int,int> > posMy;
    // 模拟的机器的下棋位子
    pair<pair<int,int>, pair<int,int> > posOther;
    // 模拟的my的下棋位子
    pair<pair<int,int>, pair<int,int> > posThir;

    // 遍历我的下棋位置可能性
    // 取出黑白棋的个数
    pair<int,int> pieceCount_bef = getPieceCount(map);
    for (int i = 0 ; i < (int)allPossible.size() ; i++){
        // 先备份棋盘状态，不要把实际的棋盘破坏了
        memcpy(myMap, map, sizeof(myMap));

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

// 棋盘输出
void printGridMap(int step,string title,int x0, int y0, int x1,int y1,int map[][7]){
    cout << endl<<"step = " << step << "   "<<  title << "   pos = " << x0 <<","<<y0<<","<<x1<<","<<y1<<endl;
    cout << "   y 1     2     3     4     5     6     7" << endl;
    cout<< "x ┌──┬──┬──┬──┬──┬──┬──┐" << endl;

    cout << "1 │";
    for(int i=0;i<=6;i++)
        if(map[0][i]==1)
            cout<<" ● │";
        else if(map[0][i]==-1)
            cout<<" ○ │";
        else
            cout << "    │";
    cout << endl;
    for(int i=1;i<=6;i++){
        cout << "  ├──┼──┼──┼──┼──┼──┼──┤" << endl;
        cout << i+1 << " │";
        for(int j=0;j<=6;j++)
            if(map[i][j]==1)
                cout<<" ● │";
            else if(map[i][j]==-1)
                cout<<" ○ │";
            else cout<<"    │";
        cout<<endl;
    }
    cout<<"  └──┴──┴──┴──┴──┴──┴──┘"<<endl;

    // 输出走棋历史记录
    if (title == "black" or title == "white") {
        vector<int> rec(6,0);
        rec[0] = step;
        rec[1] = title == "black" ?  1 : -1;
        rec[2] = x0;
        rec[3] = y0;
        rec[4] = x1;
        rec[5] = y1;
        history.push_back(rec);
    }
}


// 欢迎界面
int usage() {
    int sel;

    do{
        cout << endl << "欢迎来玩同化棋" << endl;
        cout << "0.使用方法" << endl;
        cout << "1.新开局" << endl;
        cout << "2.存盘" << endl;
        cout << "3.复盘" << endl;
        cout << "4.退出" << endl;
        cout << "请选择：";
        cin >> sel;

        // check
        if (sel != 1 and sel !=2 and sel !=3 and sel !=4) {
            cout << "*** 无效输入，请输入1-4！！！ ***" << endl;
        }

        return sel;
    }while(true);

    return 0;
}

// 使用方法
void help(){
    cout << "1 人使用黑棋，机器使用白棋" << endl;
    cout << "2 走棋输入 x0 y0 x1 y1, 表示起点位置和落子位置" << endl;
    cout << "3 x是横坐标，y是纵坐标，数值是1到7" << endl;
    cout << "2 走棋中想退出的话，输入 -1 -1 -1 -1" << endl;
}

void newGame(int begin){
    // 可能组合
    vector<pair<pair<int,int>, pair<int,int> > > allPossible;
    pair<pair<int,int>, pair<int,int> > pos;
    int posCount;

    // 下棋的步数
    int step=begin;

    // 复盘时，不输出棋盘初始状态
    if (step == 0) {
        // 新游戏的情况下，清楚历史走棋记录
           history.clear();
           // 初始化棋盘
           memset(gridInfo,0,sizeof(gridInfo));
           gridInfo[0][0] = gridInfo[6][6] = 1;  //|黑|白|
           gridInfo[6][0] = gridInfo[0][6] = -1; //|白|黑|


        // 输出棋盘初始状态
        printGridMap(0,"init",-1,-1,-1,-1,gridInfo);
    }

    do {
        // 查看用户是否还有可能走棋
        allPossible =  getAllPossibel(myColor, gridInfo);
        posCount = allPossible.size();
        // 有的情况下
        if ( posCount > 0)
        {
            bool rst=false;
            do {
                // 等待用户输入
                int x0=0,y0=0,x1=0,y1=0;
                cout << "请走棋(x0 y0 x1 y1)：";
                cin >> x0 >> y0 >> x1 >>y1;

                if ( x0 == -1)
                    return;
                rst = ProcStep(x0-1, y0-1, x1-1, y1-1, myColor, gridInfo);
                if (rst == false) {
                    cout << "*** 输入不正，请重新输入 ***" << endl;
                }
                else
                	printGridMap(++step,"black",x0,y0,x1,y1,gridInfo);
            }while(rst == false);

            // 机器走棋
            allPossible =  getAllPossibel(otherColor, gridInfo);
            posCount = allPossible.size();
            // 有的情况下
            if ( posCount > 0)
            {
                // 胡时京的贪吃算法找到一个最优解位置
                pos = husjSelect(allPossible, otherColor, gridInfo);
                //pos = randomSelect(allPossible);
                // 用该位置走一步
                one_step(pos, otherColor, gridInfo);
                // 输出结果
                printGridMap(++step,"white",pos.first.first+1,pos.first.second+1,pos.second.first+1,pos.second.second+1,gridInfo);
            }
        }

        // 黑白棋无棋可下的情况下
        if (posCount == 0) {
            // 输出最终结果
            printGridMap(step,"over",0,0,0,0,gridInfo);

            // 取出黑棋和白棋的个数
            pair<int,int> pieceCount = getPieceCount(gridInfo);
            int blackPieceCount = pieceCount.first;
            int whitePieceCount = pieceCount.second;
            // 输出黑棋和白棋的个数
            cout << "black:"<<blackPieceCount<<"---------"<<"white:"<<whitePieceCount<<endl;

            // 判断输赢
            if(blackPieceCount>whitePieceCount)
                cout<< "*** black is win ***"<< endl;
            else if(blackPieceCount<whitePieceCount)
                cout<< "*** white is win ***"<< endl;
            else
                cout<< "my god!!!"<<endl;

            cout<< "press any key to menu:"<<endl;
            getchar();
            return;
        }
    }while(true);
}

// 下棋记录存盘
void writeGame(){

     // 没下棋的情况下，没东西存
    if (history.size() == 0) {
        cout << "*** No game save ***" << endl;
        return;
    }
    ofstream out("history.txt");
    if (out.is_open()) {
        list<vector<int> >::iterator itor;

        // title
        out << left << setw(5) << "step"  << setw(5) << "b/w" << setw(3) << "xo" <<
               setw(3) << "y0" << setw(3) << "x1" << setw(3) << "y1" << endl;

        // 输出走棋步骤
        for (itor = history.begin() ; itor != history.end() ; ++itor) {
            out << left << setw(5) << (*itor)[0] << setw(5) << (*itor)[1] << setw(3) << (*itor)[2] <<
                   setw(3) << (*itor)[3] << setw(3) << (*itor)[4] << setw(3) << (*itor)[5] << endl;
        }
       out.close();
       saveFlg = true;
       cout << "*** write game OK ***" << endl;
    }else {
        cout << "*** write game error ***" << endl;
    }
}


// // 下棋记录复盘
void loadGame(){
    ifstream in("history.txt");

    char buffer[256];

    if (in.is_open()) {

        // 除去title
        in.getline (buffer,100);

        int step,color,x0,y0,x1,y1;
        while (!in.eof() )
        {
            // 读入一次走棋记录
            memset(buffer, '\0', sizeof(buffer));
            in.getline (buffer,100);
            if (strlen(buffer) == 0) break;

            // 取出记录内容，步数，颜色，起始位置，落子位置
            istringstream istr(buffer);
            vector<int> rec(6,0);
            istr >> step >> color >> x0 >> y0 >> x1 >> y1;

            // 走一步棋
            bool rcv = ProcStep(x0-1, y0-1, x1-1, y1-1, color, gridInfo);
            if (rcv == false) {
                cout << "*** 复盘记录文件错误  ***" << endl;
                cout << "*** 请重新开始游戏  ***" << endl;
                exit(-1);
            }
            printGridMap(step,color == 1 ?"black" : "white",x0,y0,x1,y1,gridInfo);
        }
       in.close();

       // 有记录的情况下
       if (step>0) {
           cout << "*** load game OK ***" << endl;
           newGame(step);
       }
       else
    	   cout << "*** No game load ***" << endl;
    }else {
        cout << "*** load game error ***" << endl;
    }
}
// exit game
bool exitGame(){
    // 不存盘退出检查
    if (history.size() > 0 and saveFlg == false) {
        cout << "*** 不存盘，退出吗？(y/n)" << endl;
        char rcv = getchar();
        if (rcv != 'y')
            return false;
    }

    cout << "you are welcome!"<< endl;
    return true;
}


int main()
{
    istream::sync_with_stdio(false);

    int ext = false;
    do{
        int sel = usage();

        switch(sel){
            case    0: //help
                help();
                break;
            case    1: //新开局
                newGame(0);
                break;
            case    2: //存盘
                writeGame();
                break;
            case    3: //复盘
                loadGame();
                break;
            case    4: //退出
                ext = exitGame();
                break;
        }
    }while(ext == false);

    return 0;
}
