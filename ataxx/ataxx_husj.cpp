bool saveFlg; // ���̷��־

// 24�����ܷ������ӵ�λ�ö���
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

// �����괦���ӣ�����Ƿ�Ϸ���ģ������
bool ProcStep(int x0, int y0, int x1, int y1, int color, int map[][7])
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

    dx = abs((x0 - x1)), dy = abs((y0 - y1));
    if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // ��֤�����ƶ���ԭ��λ�ã������ƶ�ʼ����5��5������
        return false;
    if (map[x1][y1] != 0) // ��֤�ƶ�����λ��Ϊ��
        return false;
    if (dx == 2 || dy == 2) // ����ߵ���5��5����Χ�����Ǹ���ճ��
        map[x0][y0] = 0;

    map[x1][y1] = color;
    for (dir = 0; dir < 8; dir++) // Ӱ���ڽ�8��λ��
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

// ����������Բ�ѡ������λ��
pair<pair<int,int>, pair<int,int> > randomSelect(vector<pair<pair<int,int>, pair<int,int> > > &allPossible){

    // ȡ���м��ֿ�����
    int posCount = allPossible.size();

    // ����һ�������
    srand(time(0));
    int choice = rand() % posCount;

    // ���ѡһ��λ��
    pair<int, int> beginPos = allPossible[choice].first;
    pair<int, int> possiblePos = allPossible[choice].second;

    return make_pair(beginPos, possiblePos);
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
//     ���Է�ѡȡ�Է������·�ʱ������m2���·���
//     ������m2���·����ҵ��ҵ�ǰ���ŵ��·���
//     �Ƚ�֮ǰm���·��е�ǰ���ŵ��·����ҵ�������m���·������ŵ��·���
pair<pair<int,int>, pair<int,int> > husjSelect(vector<pair<pair<int,int>, pair<int,int> > > &allPossible, int color, int map[][7]){

    int myMap[7][7] = { 0 };
    int otherMap[7][7] = { 0 };

    int goodCnt = -100;
    int badCnt = 100;

    // ����λ��
	pair<pair<int,int>, pair<int,int> > goodPossible;

	// �Է�����λ��
	pair<pair<int,int>, pair<int,int> > badPossible;

    // �ҵ�����λ��
    pair<pair<int,int>, pair<int,int> > posMy;
    // ģ��Ļ���������λ��
    pair<pair<int,int>, pair<int,int> > posOther;
    // �ҵ�����λ��2
    pair<pair<int,int>, pair<int,int> > posMy2;

    // �����ҵ�����λ�ÿ�����
    // ȡ���ڰ���ĸ���
    pair<int,int> pieceCount_bef = getPieceCount(map);

    for (int i = 0 ; i < (int)allPossible.size() ; i++){
        // �ȱ�������״̬����Ҫ��ʵ�ʵ������ƻ���
        memcpy(myMap, map, sizeof(myMap));

        // ȡ��һ��λ��
        posMy = allPossible[i];
        // ���ҵ������ϣ�ģ����һ��
        one_step(posMy, color, myMap);

        badCnt=100;

        // ȡ���Է������µ�λ��
        vector<pair<pair<int,int>, pair<int,int> > > allPossibleOther =  getAllPossibel(-color, myMap);
        for (int j = 0 ; j < (int)allPossibleOther.size() ; j++){
            // �ȱ�������״̬����Ҫ���ҵ������ƻ���
            memcpy(otherMap, myMap, sizeof(otherMap));

            // ȡ���Է������µ�һ��λ��
            posOther = allPossibleOther[j];
            // ģ����һ��
            one_step(posOther, -color, otherMap);

            // ȡ���ڰ���ĸ���
            pair<int,int> pieceCount = getPieceCount(otherMap);

            // ���Ǻ��������£�ѡ����������·�
            int cnt;
            if (color == 1)
				cnt = pieceCount.first-pieceCount.second;
				//cnt = pieceCount_bef.second-pieceCount.second;
			else
				// ���ǰ��������£�ѡ����������·�
				cnt = pieceCount.second-pieceCount.first;
				//cnt = pieceCount_bef.first-pieceCount.first;

			// ���ݵ�ǰ�Է���õ��·�
			if (cnt <= badCnt){
				badCnt = cnt;
				badPossible = posOther;
			}
        }
        //�Է��������·���һ��
        one_step(badPossible, -color,  myMap);

        // ȡ���ҿ����µ�λ��
        vector<pair<pair<int,int>, pair<int,int> > > allPossibleOther2 =  getAllPossibel(color, myMap);
        for (int j = 0 ; j < (int)allPossibleOther2.size() ; j++){
            // �ȱ�������״̬����Ҫ���ҵ������ƻ���
            memcpy(otherMap, myMap, sizeof(otherMap));

            // ȡ���ҿ����µ�һ��λ��
            posMy2 = allPossibleOther2[j];
            // ģ����һ��
            one_step(posMy2, color, otherMap);

            // ȡ���ڰ���ĸ���
            pair<int,int> pieceCount = getPieceCount(otherMap);

            // ���Ǻ��������£�ѡ����������·�
            int cnt;
            if (color == 1)
				cnt = pieceCount.first-pieceCount.second;
				//cnt = pieceCount_bef.second-pieceCount.second;
			else
				// ���ǰ��������£�ѡ����������·�
				cnt = pieceCount.second-pieceCount.first;
				//cnt = pieceCount_bef.first-pieceCount.first;

			// ��������õ��·�
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


    // ����λ��
    int goodCnt = -100;
    pair<pair<int,int>, pair<int,int> > goodPossible;

    // �ҵ�����λ��
    pair<pair<int,int>, pair<int,int> > posMy;
    // ģ��Ļ���������λ��
    pair<pair<int,int>, pair<int,int> > posOther;
    // ģ���my������λ��
    pair<pair<int,int>, pair<int,int> > posThir;

    // �����ҵ�����λ�ÿ�����
    // ȡ���ڰ���ĸ���
    pair<int,int> pieceCount_bef = getPieceCount(map);
    for (int i = 0 ; i < (int)allPossible.size() ; i++){
        // �ȱ�������״̬����Ҫ��ʵ�ʵ������ƻ���
        memcpy(myMap, map, sizeof(myMap));

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

// �������
void printGridMap(int step,string title,int x0, int y0, int x1,int y1,int map[][7]){
    cout << endl<<"step = " << step << "   "<<  title << "   pos = " << x0 <<","<<y0<<","<<x1<<","<<y1<<endl;
    cout << "   y 1     2     3     4     5     6     7" << endl;
    cout<< "x �������Щ����Щ����Щ����Щ����Щ����Щ�����" << endl;

    cout << "1 ��";
    for(int i=0;i<=6;i++)
        if(map[0][i]==1)
            cout<<" �� ��";
        else if(map[0][i]==-1)
            cout<<" �� ��";
        else
            cout << "    ��";
    cout << endl;
    for(int i=1;i<=6;i++){
        cout << "  �������੤���੤���੤���੤���੤���੤����" << endl;
        cout << i+1 << " ��";
        for(int j=0;j<=6;j++)
            if(map[i][j]==1)
                cout<<" �� ��";
            else if(map[i][j]==-1)
                cout<<" �� ��";
            else cout<<"    ��";
        cout<<endl;
    }
    cout<<"  �������ة����ة����ة����ة����ة����ة�����"<<endl;

    // ���������ʷ��¼
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


// ��ӭ����
int usage() {
    int sel;

    do{
        cout << endl << "��ӭ����ͬ����" << endl;
        cout << "0.ʹ�÷���" << endl;
        cout << "1.�¿���" << endl;
        cout << "2.����" << endl;
        cout << "3.����" << endl;
        cout << "4.�˳�" << endl;
        cout << "��ѡ��";
        cin >> sel;

        // check
        if (sel != 1 and sel !=2 and sel !=3 and sel !=4) {
            cout << "*** ��Ч���룬������1-4������ ***" << endl;
        }

        return sel;
    }while(true);

    return 0;
}

// ʹ�÷���
void help(){
    cout << "1 ��ʹ�ú��壬����ʹ�ð���" << endl;
    cout << "2 �������� x0 y0 x1 y1, ��ʾ���λ�ú�����λ��" << endl;
    cout << "3 x�Ǻ����꣬y�������꣬��ֵ��1��7" << endl;
    cout << "2 ���������˳��Ļ������� -1 -1 -1 -1" << endl;
}

void newGame(int begin){
    // �������
    vector<pair<pair<int,int>, pair<int,int> > > allPossible;
    pair<pair<int,int>, pair<int,int> > pos;
    int posCount;

    // ����Ĳ���
    int step=begin;

    // ����ʱ����������̳�ʼ״̬
    if (step == 0) {
        // ����Ϸ������£������ʷ�����¼
           history.clear();
           // ��ʼ������
           memset(gridInfo,0,sizeof(gridInfo));
           gridInfo[0][0] = gridInfo[6][6] = 1;  //|��|��|
           gridInfo[6][0] = gridInfo[0][6] = -1; //|��|��|


        // ������̳�ʼ״̬
        printGridMap(0,"init",-1,-1,-1,-1,gridInfo);
    }

    do {
        // �鿴�û��Ƿ��п�������
        allPossible =  getAllPossibel(myColor, gridInfo);
        posCount = allPossible.size();
        // �е������
        if ( posCount > 0)
        {
            bool rst=false;
            do {
                // �ȴ��û�����
                int x0=0,y0=0,x1=0,y1=0;
                cout << "������(x0 y0 x1 y1)��";
                cin >> x0 >> y0 >> x1 >>y1;

                if ( x0 == -1)
                    return;
                rst = ProcStep(x0-1, y0-1, x1-1, y1-1, myColor, gridInfo);
                if (rst == false) {
                    cout << "*** ���벻�������������� ***" << endl;
                }
                else
                	printGridMap(++step,"black",x0,y0,x1,y1,gridInfo);
            }while(rst == false);

            // ��������
            allPossible =  getAllPossibel(otherColor, gridInfo);
            posCount = allPossible.size();
            // �е������
            if ( posCount > 0)
            {
                // ��ʱ����̰���㷨�ҵ�һ�����Ž�λ��
                pos = husjSelect(allPossible, otherColor, gridInfo);
                //pos = randomSelect(allPossible);
                // �ø�λ����һ��
                one_step(pos, otherColor, gridInfo);
                // ������
                printGridMap(++step,"white",pos.first.first+1,pos.first.second+1,pos.second.first+1,pos.second.second+1,gridInfo);
            }
        }

        // �ڰ���������µ������
        if (posCount == 0) {
            // ������ս��
            printGridMap(step,"over",0,0,0,0,gridInfo);

            // ȡ������Ͱ���ĸ���
            pair<int,int> pieceCount = getPieceCount(gridInfo);
            int blackPieceCount = pieceCount.first;
            int whitePieceCount = pieceCount.second;
            // �������Ͱ���ĸ���
            cout << "black:"<<blackPieceCount<<"---------"<<"white:"<<whitePieceCount<<endl;

            // �ж���Ӯ
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

// �����¼����
void writeGame(){

     // û���������£�û������
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

        // ������岽��
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


// // �����¼����
void loadGame(){
    ifstream in("history.txt");

    char buffer[256];

    if (in.is_open()) {

        // ��ȥtitle
        in.getline (buffer,100);

        int step,color,x0,y0,x1,y1;
        while (!in.eof() )
        {
            // ����һ�������¼
            memset(buffer, '\0', sizeof(buffer));
            in.getline (buffer,100);
            if (strlen(buffer) == 0) break;

            // ȡ����¼���ݣ���������ɫ����ʼλ�ã�����λ��
            istringstream istr(buffer);
            vector<int> rec(6,0);
            istr >> step >> color >> x0 >> y0 >> x1 >> y1;

            // ��һ����
            bool rcv = ProcStep(x0-1, y0-1, x1-1, y1-1, color, gridInfo);
            if (rcv == false) {
                cout << "*** ���̼�¼�ļ�����  ***" << endl;
                cout << "*** �����¿�ʼ��Ϸ  ***" << endl;
                exit(-1);
            }
            printGridMap(step,color == 1 ?"black" : "white",x0,y0,x1,y1,gridInfo);
        }
       in.close();

       // �м�¼�������
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
    // �������˳����
    if (history.size() > 0 and saveFlg == false) {
        cout << "*** �����̣��˳���(y/n)" << endl;
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
            case    1: //�¿���
                newGame(0);
                break;
            case    2: //����
                writeGame();
                break;
            case    3: //����
                loadGame();
                break;
            case    4: //�˳�
                ext = exitGame();
                break;
        }
    }while(ext == false);

    return 0;
}
