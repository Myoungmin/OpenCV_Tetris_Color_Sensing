#include <opencv2/opencv.hpp> 
#include <iostream>
#include <time.h>

using namespace std;
using namespace cv;
using namespace ml;	//머신러닝 네임스페이스 사용

Mat img;
Mat key;

Mat UP;
Mat DOWN;
Mat LEFT;
Mat RIGHT;

int button = 0;
int clear = 1;

enum{up_ = 1, down_, left_, right_};



const unsigned char tetriminos[7][4][16] = { // ㅁ
	{ { 0,2,2,0,
	0,2,2,0,
	0,0,0,0,
	0,0,0,0 },
	{ 0,2,2,0,
	0,2,2,0,
	0,0,0,0,
	0,0,0,0 },
	{ 0,2,2,0,
	0,2,2,0,
	0,0,0,0,
	0,0,0,0 },
	{ 0,2,2,0,
	0,2,2,0,
	0,0,0,0,
	0,0,0,0 } },
	// ㅗ
	{ { 0,3,0,0,
	3,3,3,0,
	0,0,0,0,
	0,0,0,0 },
	{ 0,3,0,0,
	0,3,3,0,
	0,3,0,0,
	0,0,0,0 },
	{ 0,0,0,0,
	3,3,3,0,
	0,3,0,0,
	0,0,0,0 },
	{ 0,3,0,0,
	3,3,0,0,
	0,3,0,0,
	0,0,0,0 } },
	// ㄴ
	{ { 4,0,0,0,
	4,4,4,0,
	0,0,0,0,
	0,0,0,0 },
	{ 4,4,0,0,
	4,0,0,0,
	4,0,0,0,
	0,0,0,0 },
	{ 4,4,4,0,
	0,0,4,0,
	0,0,0,0,
	0,0,0,0 },
	{ 0,4,0,0,
	0,4,0,0,
	4,4,0,0,
	0,0,0,0 } },
	// ┘
	{ { 0,0,5,0,
	5,5,5,0,
	0,0,0,0,
	0,0,0,0 },
	{ 5,0,0,0,
	5,0,0,0,
	5,5,0,0,
	0,0,0,0 },
	{ 5,5,5,0,
	5,0,0,0,
	0,0,0,0,
	0,0,0,0 },
	{ 5,5,0,0,
	0,5,0,0,
	0,5,0,0,
	0,0,0,0 } },
	//ㅡ┐
	//  ┕ㅡ
	{ { 6,6,0,0,
	0,6,6,0,
	0,0,0,0,
	0,0,0,0 },
	{ 0,6,0,0,
	6,6,0,0,
	6,0,0,0,
	0,0,0,0 },
	{ 6,6,0,0,
	0,6,6,0,
	0,0,0,0,
	0,0,0,0 },
	{ 0,6,0,0,
	6,6,0,0,
	6,0,0,0,
	0,0,0,0 } },
	//   ┎ㅡ
	// ㅡ┛
	{ { 0,7,7,0,
	7,7,0,0,
	0,0,0,0,
	0,0,0,0 },
	{ 7,0,0,0,
	7,7,0,0,
	0,7,0,0,
	0,0,0,0 },
	{ 0,7,7,0,
	7,7,0,0,
	0,0,0,0,
	0,0,0,0 },
	{ 7,0,0,0,
	7,7,0,0,
	0,7,0,0,
	0,0,0,0 } },
	// ㅡㅡㅡㅡ 짝대기
	{ { 8,8,8,8,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0 },
	{ 0,8,0,0,
	0,8,0,0,
	0,8,0,0,
	0,8,0,0 },
	{ 8,8,8,8,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0 },
	{ 0,8,0,0,
	0,8,0,0,
	0,8,0,0,
	0,8,0,0 } } };


unsigned int shape;                  // 테트리미노스의 7가지 모양
unsigned int pattern;                 // 테트리미노스의 4가지 패턴
unsigned int cur_line;               // 테트리니노스의 현재 라인
unsigned int cur_col;                // 테트리니노스의 현재 칸
unsigned int temp_line[4][12];          // 테트리미노스 라인 임시 저장소
unsigned int main_board[32][12] = { 0 };	//테르리미노스가 굳어진 후 저장된 게임보드
unsigned int game_board[32][12];	//테트리미노스가 움직이면서 변화하는 게임보드
unsigned int crush = 0;	//부딪힘을 나타내는 플레그
unsigned int new_block = 0;	//새로운 블록이 생성되야함을 나타내는 플레그
unsigned int game_over = 0;	//게임이 종료되었음을 나타내는 플레그
unsigned int next_block = 0;
unsigned int next_board[4][4] = { 0 };
unsigned int red;
unsigned int blue;
unsigned int green;
int b;
int g;
int r;


unsigned char Collision()	//충돌 여부 확인
{
	for (int i = 0; i < 12; i++)
	{
		//여기 수정하는데 오래걸림 이전 버전처럼 비트연산하면 짝수인 블록이 0이 되버려서 벽을 통과해버린다!!!
		if (((main_board[cur_line][i] && temp_line[0][i]) != 0) | ((main_board[cur_line + 1][i] && temp_line[1][i]) != 0) |
			((main_board[cur_line + 2][i] && temp_line[2][i]) != 0) | ((main_board[cur_line + 3][i] && temp_line[3][i]) != 0))
			return 1;         // 충돌 1 리턴
	}
	return 0;  // 충돌 없음 0 리턴
}


void tetriminos_to_temp_line()
{
	for (int i = 0; i < 4; i++)	//테트리미노스 라인 임시 저장소 초기화
	{
		for (int j = 0; j < 12; j++)
		{
			temp_line[i][j] = 0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (cur_col + i <= 11) temp_line[0][cur_col + i] = tetriminos[shape][pattern][i];
		if (cur_col + i <= 11) temp_line[1][cur_col + i] = tetriminos[shape][pattern][i + 4];
		if (cur_col + i <= 11) temp_line[2][cur_col + i] = tetriminos[shape][pattern][i + 8];
		if (cur_col + i <= 11) temp_line[3][cur_col + i] = tetriminos[shape][pattern][i + 12];
	}

}


void draw_map()
{
	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 12; j++)
		{
			if (game_board[i][j] == 1)	rectangle(img, Point(10 * j, 10 * i), Point(10 * j + 9, 10 * i + 9), Scalar(blue, green, red), -1);
			else if (game_board[i][j] == 2) rectangle(img, Point(10 * j, 10 * i), Point(10 * j + 9, 10 * i + 9), Scalar(255, 0, 100), -1);
			else if (game_board[i][j] == 3) rectangle(img, Point(10 * j, 10 * i), Point(10 * j + 9, 10 * i + 9), Scalar(255, 5, 0), -1);
			else if (game_board[i][j] == 4) rectangle(img, Point(10 * j, 10 * i), Point(10 * j + 9, 10 * i + 9), Scalar(255, 0, 0), -1);
			else if (game_board[i][j] == 5) rectangle(img, Point(10 * j, 10 * i), Point(10 * j + 9, 10 * i + 9), Scalar(0, 255, 0), -1);
			else if (game_board[i][j] == 6) rectangle(img, Point(10 * j, 10 * i), Point(10 * j + 9, 10 * i + 9), Scalar(0, 255, 255), -1);
			else if (game_board[i][j] == 7) rectangle(img, Point(10 * j, 10 * i), Point(10 * j + 9, 10 * i + 9), Scalar(0, 100, 255), -1);
			else if (game_board[i][j] == 8) rectangle(img, Point(10 * j, 10 * i), Point(10 * j + 9, 10 * i + 9), Scalar(0, 0, 255), -1);
		}
	}




	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (next_board[i][j] == 1)	rectangle(img, Point(10 * j, 10 * i), Point(10 * j + 9, 10 * i + 9), Scalar(blue, green, red), -1);
			else if (next_board[i][j] == 2) rectangle(img, Point(130 + 10 * i, 10 * j), Point(130 + 10 * i + 9, 10 * j + 9), Scalar(255, 0, 100), -1);
			else if (next_board[i][j] == 3) rectangle(img, Point(130 + 10 * i, 10 * j), Point(130 + 10 * i + 9, 10 * j + 9), Scalar(255, 5, 0), -1);
			else if (next_board[i][j] == 4) rectangle(img, Point(130 + 10 * i, 10 * j), Point(130 + 10 * i + 9, 10 * j + 9), Scalar(255, 0, 0), -1);
			else if (next_board[i][j] == 5) rectangle(img, Point(130 + 10 * i, 10 * j), Point(130 + 10 * i + 9, 10 * j + 9), Scalar(0, 255, 0), -1);
			else if (next_board[i][j] == 6) rectangle(img, Point(130 + 10 * i, 10 * j), Point(130 + 10 * i + 9, 10 * j + 9), Scalar(0, 255, 255), -1);
			else if (next_board[i][j] == 7) rectangle(img, Point(130 + 10 * i, 10 * j), Point(130 + 10 * i + 9, 10 * j + 9), Scalar(0, 100, 255), -1);
			else if (next_board[i][j] == 8) rectangle(img, Point(130 + 10 * i, 10 * j), Point(130 + 10 * i + 9, 10 * j + 9), Scalar(0, 0, 255), -1);
		}
	}
}

void NewTetriminos()
{
	new_block = 0;	//새로운 블록 플레그 끄기
	shape = next_block;
	next_block = rand() % 7;					//다음에 올 테트리미노스 랜덤 출력
	pattern = 0;	//기본 회전모향 설정
	cur_line = 0;                 // 테트리미노스 현재 라인 (최상위 라인)
	cur_col = 6;                // 테트리미노스의 현재 칸

	tetriminos_to_temp_line();

	for (int i = 0; i < 12; i++)
	{
		game_over |= game_board[cur_line][i] & temp_line[0][i];	//게임보드에 있는 테트리미노스와 임시저장소에 생긴 테트리미노스가 겹치는지 확인하고 겹치면 게임오버 플레그 켜짐
		game_over |= game_board[cur_line + 1][i] & temp_line[1][i];	//or 연산으로 어디든 겹치면 플레그 켜진다
		game_over |= game_board[cur_line + 2][i] & temp_line[2][i];
		game_over |= game_board[cur_line + 3][i] & temp_line[3][i];

		if (game_over != 0) break;
	}

	for (int i = 1; i < 11; i++)
	{
		game_board[cur_line][i] |= temp_line[0][i];	//현재라인 아래로 이동 후 변화하는 보드에 반영
		game_board[cur_line + 1][i] |= temp_line[1][i];
		game_board[cur_line + 2][i] |= temp_line[2][i];
		game_board[cur_line + 3][i] |= temp_line[3][i];
	}
}


void NextTetriminos()
{
	for (int i = 0; i < 4; i++)	//넥스트 보드 초기화
	{
		for (int j = 0; j < 4; j++)
		{
			next_board[i][j] = 0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		next_board[i][0] = tetriminos[next_block][0][i];
		next_board[i][1] = tetriminos[next_block][0][i + 4];
		next_board[i][2] = tetriminos[next_block][0][i + 8];
		next_board[i][3] = tetriminos[next_block][0][i + 12];
	}
}


int main() {
	srand((unsigned int)time(NULL));

	VideoCapture cap(0);	//비디오 캡쳐




	while (1)
	{
		game_over = 0;	//게임종료 플레그 끄기


		blue = rand() % 256;
		green = rand() % 256;
		red = rand() % 256;

		for (int i = 0; i < 32; i++)	//메인보드 초기화
		{
			for (int j = 0; j < 12; j++)
			{
				if (i == 31 || j == 0 || j == 11) main_board[i][j] = 1;
				else main_board[i][j] = 0;
			}
		}
		new_block = 1;

		int width = cvRound(cap.get(CAP_PROP_FRAME_WIDTH));
		int height = cvRound(cap.get(CAP_PROP_FRAME_HEIGHT));
		double fps = 1000 / 200;	//delay를 200ms로 만들어서 그에따른 프레임 계산
		int fourcc = VideoWriter::fourcc('D', 'I', 'V', 'X');
		VideoWriter writer("output.avi", fourcc, fps, Size(width, height));

		while (game_over == 0)	//게임종료 플레그가 꺼저있을동안 반복
		{
			int button_size = 80;

			cap >> img;	//비디오 캡쳐 영상으로

			flip(img, img, 1);	//비디오 좌우 반전

			

			key = img.clone();
			cvtColor(key, key, COLOR_BGR2HSV);
			inRange(key, Scalar(0, 30, 0), Scalar(15, 255, 255), key);
			morphologyEx(key, key, MORPH_ERODE, Mat());
			morphologyEx(key, key, MORPH_ERODE, Mat());
			morphologyEx(key, key, MORPH_CLOSE, Mat());
			morphologyEx(key, key, MORPH_DILATE, Mat());
			morphologyEx(key, key, MORPH_DILATE, Mat());
			morphologyEx(key, key, MORPH_DILATE, Mat());
			morphologyEx(key, key, MORPH_DILATE, Mat());
			morphologyEx(key, key, MORPH_DILATE, Mat());

			

			UP = key(Rect(img.cols - (button_size * 4), img.rows / 2 - (button_size * 3 / 2), button_size, button_size));
			DOWN = key(Rect(img.cols - (button_size * 4), img.rows / 2 + (button_size * 3 / 2), button_size, button_size));
			LEFT = key(Rect(img.cols - (button_size * 6), img.rows / 2, button_size, button_size));
			RIGHT = key(Rect(img.cols - (button_size * 2), img.rows / 2, button_size, button_size));

			vector<vector<Point>> contours_UP, contours_DOWN, contours_LEFT, contours_RIGHT;
			findContours(UP, contours_UP, RETR_LIST, CHAIN_APPROX_NONE);
			findContours(DOWN, contours_DOWN, RETR_LIST, CHAIN_APPROX_NONE);
			findContours(LEFT, contours_LEFT, RETR_LIST, CHAIN_APPROX_NONE);
			findContours(RIGHT, contours_RIGHT, RETR_LIST, CHAIN_APPROX_NONE);

			cvtColor(UP, UP, COLOR_GRAY2BGR);
			cvtColor(DOWN, DOWN, COLOR_GRAY2BGR);
			cvtColor(LEFT, LEFT, COLOR_GRAY2BGR);
			cvtColor(RIGHT, RIGHT, COLOR_GRAY2BGR);

			for (int i = 0; i < contours_UP.size(); i++) {
				if (button != 0 || clear == 0) break;
				else if (contourArea(contours_UP[i]) > 300 && button == 0 && clear == 1)
				{
					drawContours(UP, contours_UP, i, Scalar(0, 255, 0), -1);
					button = up_;
					clear = 0;
				}
			}

			

			for (int i = 0; i < contours_LEFT.size(); i++) {
				if (button != 0 || clear == 0) break;
				else if (contourArea(contours_LEFT[i]) > 300 && button == 0 && clear == 1)
				{
					drawContours(LEFT, contours_LEFT, i, Scalar(0, 255, 0), -1);
					button = left_;
					clear = 0;
				}
			}

			for (int i = 0; i < contours_RIGHT.size(); i++) {
				if (button != 0 || clear == 0) break;
				else if (contourArea(contours_RIGHT[i]) > 300 && button == 0 && clear == 1)
				{
					drawContours(RIGHT, contours_RIGHT, i, Scalar(0, 255, 0), -1);
					button = right_;
					clear = 0;
				}
			}
			
			for (int i = 0; i < contours_DOWN.size(); i++) {
				if (button != 0 || clear == 0) break;
				else if (contourArea(contours_DOWN[i]) > 300 && button == 0 && clear == 1)
				{
					drawContours(DOWN, contours_DOWN, i, Scalar(0, 255, 0), -1);
					button = down_;
					clear = 0;
				}
			}

			static int count = 0;	
			//clear 변수를 써도 너무 입력이 자주되어 button입력이 안되는것 여러번 반복되면 입력되게 바꿈 
			if (button == 0) count++;
			if (count == 2) {
				count = 0;
				clear = 1;
			}

			int button_color = 0;
			int button_line = 2;

			//입력되면 네모 색 두께 바뀌게
			if (button == up_) {
				button_color = 255;
				button_line = 5;
			}
			rectangle(img, Rect(img.cols - (button_size * 4), img.rows/2 - (button_size * 3 / 2), button_size, button_size), Scalar(button_color, 0, 255), button_line);
			button_color = 0;
			button_line = 2;

			if (button == left_) {
				button_color = 255;
				button_line = 5;
			}
			rectangle(img, Rect(img.cols - (button_size * 6), img.rows / 2, button_size, button_size), Scalar(button_color, 0, 255), button_line);
			button_color = 0;
			button_line = 2;

			if (button == right_) {
				button_color = 255;
				button_line = 5;
			}
			rectangle(img, Rect(img.cols - (button_size * 2), img.rows/2, button_size, button_size), Scalar(button_color, 0, 255), button_line);
			button_color = 0;
			button_line = 2;

			if (button == down_) {
				button_color = 255;
				button_line = 5;
			}
			rectangle(img, Rect(img.cols - (button_size * 4), img.rows / 2 + (button_size * 3 / 2), button_size, button_size), Scalar(button_color, 0, 255), button_line);
			button_color = 0;
			button_line = 2;


			if (new_block == 1)
			{
				for (int i = 0; i < 31; i++)
				{
					int count = 0;
					for (int j = 1; j < 11; j++)
					{
						if (main_board[i][j] == 0) break;
						count++;	//1줄이 모두 완성되어서 깨질 줄이 있는지 확인
					}
					if (count == 10)
					{
						for (int k = i; k > 0; k--)
						{
							for (int j = 1; j < 11; j++)
							{
								main_board[k][j] = main_board[k - 1][j];	//깨진 줄의 위에 줄들 아래로 이동(행렬상 열 증가)
							}
						}
					}
				}

				for (int i = 0; i < 32; i++)
				{
					for (int j = 0; j < 12; j++)
					{
						game_board[i][j] = main_board[i][j];	//굳어진후 저장된 보드를 변화하는 보드로 복사
					}
				}
				NewTetriminos();	//새로운 테트리미노스 생성
				NextTetriminos();
			}

			if (new_block == 0)
			{
				draw_map();	//반영된 변화하는 보드 화면으로 출력
				for (int i = 0; i < 32; i++)
				{
					for (int j = 0; j < 12; j++)
					{
						game_board[i][j] = main_board[i][j];	//굳어진후 저장된 보드를 변화하는 보드로 복사
					}
				}

				cur_line++;	//현재라인 아래로 이동

				if (Collision() == 1)	//이동후 충돌 발생시
				{
					cur_line--;	//원래 라인으로 복귀
					for (int j = 1; j < 11; j++)
					{
						main_board[cur_line][j] |= temp_line[0][j];	//현재라인 아래로 이동 후 변화하는 보드에 반영
						main_board[cur_line + 1][j] |= temp_line[1][j];
						main_board[cur_line + 2][j] |= temp_line[2][j];
						main_board[cur_line + 3][j] |= temp_line[3][j];
					}


					new_block = 1;	//새로운 블록 플레그 켜짐
				}

				for (int i = 0; i < 32; i++)
				{
					for (int j = 0; j < 12; j++)
					{
						game_board[i][j] = main_board[i][j];	//굳어진후 저장된 보드를 변화하는 보드로 복사
					}
				}

				for (int j = 1; j < 11; j++)
				{
					game_board[cur_line][j] |= temp_line[0][j];	//현재라인 아래로 이동 후 변화하는 보드에 반영
					game_board[cur_line + 1][j] |= temp_line[1][j];
					game_board[cur_line + 2][j] |= temp_line[2][j];
					game_board[cur_line + 3][j] |= temp_line[3][j];
				}

			}

			imshow("src", img);

			writer.write(img);

			imshow("key", key);

			imshow("UP", UP);
			imshow("DOIWN", DOWN);
			imshow("LEFT", LEFT);
			imshow("RIGHT", RIGHT);
		
			

			if(waitKeyEx(200) == 27) return 0;

			switch (button)
			{
			case left_:
				for (int i = 0; i < 32; i++)
				{
					for (int j = 0; j < 12; j++)
					{
						game_board[i][j] = main_board[i][j];	//굳어진후 저장된 보드를 변화하는 보드로 복사
					}
				}

				if (cur_col > 0) cur_col--;


				tetriminos_to_temp_line();

				if (Collision() == 1) cur_col++;

				tetriminos_to_temp_line();

				for (int j = 1; j < 11; j++)
				{
					game_board[cur_line][j] |= temp_line[0][j];	//현재라인 아래로 이동 후 변화하는 보드에 반영
					game_board[cur_line + 1][j] |= temp_line[1][j];
					game_board[cur_line + 2][j] |= temp_line[2][j];
					game_board[cur_line + 3][j] |= temp_line[3][j];
				}
				break;

			case up_:
				for (int i = 0; i < 32; i++)
				{
					for (int j = 0; j < 12; j++)
					{
						game_board[i][j] = main_board[i][j];	//굳어진후 저장된 보드를 변화하는 보드로 복사
					}
				}

				pattern++;	//회전으로 상태 변화
				if (pattern == 4) pattern = 0; //마지막에서 처음으로

				tetriminos_to_temp_line();

				if (Collision() == 1) pattern--;


				tetriminos_to_temp_line();

				for (int j = 1; j < 11; j++)
				{
					game_board[cur_line][j] |= temp_line[0][j];	//현재라인 아래로 이동 후 변화하는 보드에 반영
					game_board[cur_line + 1][j] |= temp_line[1][j];
					game_board[cur_line + 2][j] |= temp_line[2][j];
					game_board[cur_line + 3][j] |= temp_line[3][j];
				}
				break;

			case down_:
				for (int i = 0; i < 32; i++)
				{
					for (int j = 0; j < 12; j++)
					{
						game_board[i][j] = main_board[i][j];	//굳어진후 저장된 보드를 변화하는 보드로 복사
					}
				}

				tetriminos_to_temp_line();

				while (Collision() == 0) cur_line++;

				cur_line--;

				tetriminos_to_temp_line();

				for (int j = 1; j < 11; j++)
				{
					game_board[cur_line][j] |= temp_line[0][j];	//현재라인 아래로 이동 후 변화하는 보드에 반영
					game_board[cur_line + 1][j] |= temp_line[1][j];
					game_board[cur_line + 2][j] |= temp_line[2][j];
					game_board[cur_line + 3][j] |= temp_line[3][j];
				}
				break;

			case right_:
				for (int i = 0; i < 32; i++)
				{
					for (int j = 0; j < 12; j++)
					{
						game_board[i][j] = main_board[i][j];	//굳어진후 저장된 보드를 변화하는 보드로 복사
					}
				}

				if (cur_col < 12) cur_col++;

				tetriminos_to_temp_line();

				if (Collision() == 1) cur_col--;

				tetriminos_to_temp_line();


				for (int j = 1; j < 11; j++)
				{
					game_board[cur_line][j] |= temp_line[0][j];	//현재라인 아래로 이동 후 변화하는 보드에 반영
					game_board[cur_line + 1][j] |= temp_line[1][j];
					game_board[cur_line + 2][j] |= temp_line[2][j];
					game_board[cur_line + 3][j] |= temp_line[3][j];
				}
				break;
			default:
				break;
			}

			button = 0;
		}
	}
}

