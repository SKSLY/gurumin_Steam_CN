// SortApplication.cpp: 定义控制台应用程序的入口点。
//
 
#include "stdafx.h"
#include <regex> 
#include <string> 
#include <vector> 
#include <iostream> 
#include <fstream> 
#include <iomanip> 
#include <sstream>  
using namespace std;

//正则参考自  https://github.com/fengbingchun/Messy_Test
int test_regex_replace()
{
	//std::string pattern{ "\\d{18}|\\d{17}X" }; // id card 
	std::string pattern{ "\\S+=" };	//多次匹配任意字符（\S+）之后跟=
	std::regex re(pattern);
	std::vector<std::string> str{ "b0a3=埃", "816D=［",
		"E141=瓣", "12345678901234567X" };
	std::string fmt{ "" };
	/* std::regex_replace:
	在整个字符序列中查找正则表达式re的所有匹配
	这个算法每次成功匹配后，就根据参数fmt对匹配字符串进行替换
	*/
	for (auto tmp : str) {
		std::string ret = std::regex_replace(tmp, re, fmt);
		fprintf(stderr, "src: %s, dst: %s\n", tmp.c_str(), ret.c_str());
	}
	return 0;
}

void test_rawcopy() {
	ifstream jfont_in("Files/J_ddfont8.dat", ios::in | ios::binary | ios::ate);
	ifstream cfont_in("Files/C_ddfont8.dat", ios::in | ios::binary | ios::ate);


	//cout << "the complete file is in a buffer" << endl;

	ofstream fout("Files/copytest.dat", ios::binary);

	char szBuf[16 * 9] = { 0 };

	int counttemp = 0;
	while (counttemp < 1000)
	{
		jfont_in.seekg(counttemp * 16 * 9, ios::beg);
		counttemp++;

		jfont_in.read(szBuf, sizeof(char) * 16 * 9);

		fout.write(szBuf, sizeof(char) * 16 * 9);
	}
	cout << "写入完毕" << endl;
	jfont_in.close();
	fout.close();
}
string table_regex_replace(string raw)
{
	string pattern{ "\\S+=" };	//多次匹配任意字符（\S+）之后跟=
	regex rec(pattern);
	string fmt{ "" };
	/* std::regex_replace:
	在整个字符序列中查找正则表达式rec的所有匹配
	这个算法每次成功匹配后，就根据参数fmt对匹配字符串进行替换
	*/

	std::string ret = std::regex_replace(raw, rec, fmt);

	return ret;
}
string table_regex_replace_back(string raw)
{
	string pattern{ "=+\\S" };	//多次匹配任意字符（\S+）之后跟=
	regex rec(pattern);
	string fmt{ "" };

	std::string ret = std::regex_replace(raw, rec, fmt);

	return ret;
}

string dec2hex(int i) //将int转成16进制字符串  from http://blog.csdn.net/nwpu_yike/article/details/22100615
{
	stringstream ioss; //定义字符串流  
	string s_temp; //存放转化后字符  
	ioss << setiosflags(ios::uppercase) << hex << i; //以十六制(大写)形式输出  
													 //ioss << resetiosflags(ios::uppercase) << hex << i; //以十六制(小写)形式输出//取消大写的设置  
	ioss >> s_temp;
	return s_temp;
}
void tablecomp() {
	ifstream table_c;
	ifstream table_j;
	table_c.open("Files/GB2JIS.txt", ios::in);
	table_j.open("Files/Shift-JIS-A_mini_edit.txt", ios::in);

	ifstream jfont_in("Files/J_ddfont8.dat", ios::in | ios::binary | ios::ate);
	ifstream cfont_in("Files/C_ddfont8.dat", ios::in | ios::binary | ios::ate);
	ofstream fout("Files/copytest.dat", ios::binary);

	char szBuf[16 * 9] = { 0 };

	string str;
	string str2;

	int countj = 33089;	//日文字库应有长度为EEEC-8141,正常码表有精简，需补回精简部分生成空值
	//int countj = 56000;	//日文字库应有长度为EEEC-8141,正常码表有精简，需补回精简部分生成空值

	while (countj<=61164)
	{
		countj++;

		streampos temppos = table_j.tellg();
		getline(table_j, str);

		int countc = 0;

		//根据现有字库与码表做出的特殊调整，跳过9FFC到E040间的空隙
		if (strncmp(table_regex_replace_back(str).c_str(), "E040", 4) == 0) {
			countj = 57408+1;
			cout << "界限已过！" << endl;
			jfont_in.clear();
		}

		//如果没有获取到应有的行号，补空位，读取日文码表的指针返回一行
		//因为正则或者转换16进制过程可能有不可见字符影响，使用strncmp单独比较前4位字符
		if (strncmp(table_regex_replace_back(str).c_str(),(dec2hex(countj-1)).c_str(),4)!=0 ) {
			cout << table_regex_replace_back(str)<< endl;
			//cout <<  dec2hex(countj-1) << endl;
			char szBuf[16 * 9] = { 0 };//安全起见清空一次
			jfont_in.clear();
			jfont_in.seekg(0, ios::beg);
			jfont_in.seekg(109 * 16 * 9, ios::beg);
			jfont_in.read(szBuf, sizeof(char) * 16 * 9);
			fout.write(szBuf, sizeof(char) * 16 * 9);
			cout << "补空位！！" << endl;
			table_j.seekg(temppos, ios::beg);
			table_c.seekg(0, ios::beg);
		}
		else {

			if (table_regex_replace(str) == "null") {
				//字库buffer结尾追加日版字库对应
				//jfont_in.seekg(0, ios::beg);
				char szBuf[16 * 9] = { 0 };//安全起见清空一次
				jfont_in.clear();
				if (countj >= 57408) {
					jfont_in.seekg((countj - 33089 - 16451 - 1) * 16 * 9, ios::beg);

				}
				else {
					jfont_in.seekg((countj - 33089 - 1) * 16 * 9, ios::beg);
				}
				jfont_in.read(szBuf, sizeof(char) * 16 * 9);
				fout.write(szBuf, sizeof(char) * 16 * 9);
				cout << "J:" + table_regex_replace(str) << endl;
			}
			else {
				while (countc<8178)
				{
				
					countc++;
					getline(table_c, str2);
					if (table_regex_replace(str) == table_regex_replace(str2)) {
						//cout << table_regex_replace(str2) << endl;
						//字库buffer结尾追加汉化版字库相应位置的数据
						cfont_in.seekg((countc-1) * 16 * 9, ios::beg);
						cfont_in.read(szBuf, sizeof(char) * 16 * 9);
						fout.write(szBuf, sizeof(char) * 16 * 9);
						cout << "C:"+table_regex_replace(str2) << endl;
						table_c.seekg(0, ios::beg);

						break;
					}
					else {
						//扫描中文码表下一行

					}
				}
				if (countc >= 8178) {
					//字库buffer结尾追加日版字库相应位置的数据
					//jfont_in.seekg(0, ios::beg);
					char szBuf[16 * 9] = { 0 };//安全起见清空一次
					jfont_in.clear();
					if (countj >= 57408) {
						jfont_in.seekg((countj - 33089 - 16451 - 1) * 16 * 9, ios::beg);

					}
					else {
						jfont_in.seekg((countj - 33089 - 1) * 16 * 9, ios::beg);
					}
					jfont_in.read(szBuf, sizeof(char) * 16 * 9);
					fout.write(szBuf, sizeof(char) * 16 * 9);
					table_c.seekg(0, ios::beg);

					cout << "J:" + table_regex_replace(str) << endl;
				}
			}
		}
	}

	cout << "写入完毕" << endl;
	jfont_in.close();
	cfont_in.close();
	fout.close();

	table_c.close();
	table_j.close();
	//fprintf(stderr, "读取结果: %d\n ",table_c.is_open());
	return ;
}
int main() {
	tablecomp();
	//test_rawcopy();
	/*string str = "814C=′";
	cout << table_regex_replace_back(str) << endl;*/


	return 0;

}
