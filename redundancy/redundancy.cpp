#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
using namespace std;

typedef unsigned int	u32;
typedef unsigned short	u16;
typedef unsigned char	u8;

constexpr auto BUF_SIZE = 1024;
typedef struct {
	u32 ip;
	u32 Prefix;
	u32 portx[2];
	u32 porty[2];
	u32 resule;
}MES;

typedef struct {
	u32 s;
	vector<u32> d;
}Redu;


u32 rectangle(u32 p[4]) {
	if (p[1] < p[2] || p[3] < p[0]) return 0; //无交集
	u32 temp;
	for (int i = 0; i < 3; i++) {
		for (int j = i + 1; j < 4; j++) {
			if (p[j] < p[i]) {
				temp = p[j];
				p[j] = p[i];
				p[i] = temp;
			}
		}
	}
	return 1;
}

u32 intersection(MES m1, MES m2, MES &mes) {  // 0 无交集  1 同类有交集 2 异类有交集 
	if (m1.resule == 2 || m2.resule == 2) return 0;
	MES m[2] = {m1,m2};
	bool flagResule = (m[0].resule == m[1].resule) ? 0 : 1;
	u32 p = (m1.Prefix <= m2.Prefix) ? 0 : 1;
	u32 q = (m1.Prefix <= m2.Prefix) ? 1 : 0;
	u32 ip1 = (m1.ip & (0xffffffff << (32 - m[p].Prefix)));
	u32 ip2 = (m2.ip & (0xffffffff << (32 - m[p].Prefix)));
	if (ip1 == ip2) {
		mes.ip = m[q].ip;
		mes.Prefix = m[q].Prefix;
	}
	else {
		return 0;  //无交集
	}
	u32 px[4] = { m1.portx[0], m1.portx[1], m2.portx[0], m2.portx[1] };
	if (rectangle(px) != 0) {
		mes.portx[0] = px[1];
		mes.portx[1] = px[2];
	}
	else {
		return 0;
	}
	u32 py[4] = { m1.porty[0], m1.porty[1], m2.porty[0], m2.porty[1] };
	if (rectangle(py) != 0) {
		mes.porty[0] = py[1];
		mes.porty[1] = py[2];
	}
	else {
		return 0;
	}
	if (flagResule == 0)
		return 1;
	else
		return 2;
}

int main()
{
    std::cout << "Hello World!\n"; 
	clock_t startTime, endTime;
	startTime = clock();
	vector<Redu> redu;
	FILE *fr, *fw;
	if ((fr = fopen("rule3.txt", "r")) == NULL || (fw = fopen("out.txt", "w")) == NULL) {
		cout << "can not open the file." << endl;
		exit(0);
	}
	u32 NumRule = 0, CountRule = 0;
	vector<MES> mes;
	MES m;
	fscanf(fr, "%d\n", &NumRule);
	while (CountRule++< NumRule) {
		u32 ip1, ip2, ip3, ip4;
		fscanf(fr, "%d.%d.%d.%d/%d %d:%d %d:%d %d\n", &ip1, &ip2, &ip3, &ip4, &m.Prefix,
			&m.portx[0], &m.portx[1], &m.porty[0], &m.porty[1], &m.resule);//1.1.2.0/24   100:236   1:155 1
		m.ip = (ip1 << 24) | (ip2 << 16) | (ip3 << 8) | (ip4);
		mes.push_back(m);
	}

	for (int i = NumRule - 1; i >= 0; i--) {
		u32 pi = mes[i].ip & (0xffffffff << (32 - mes[i].Prefix));
		u32 li = pow(2,32- mes[i].Prefix);
		u32 px = mes[i].portx[0];
		u32 lx = mes[i].portx[1] - mes[i].portx[0] + 1;
		u32 py = mes[i].porty[0];
		u32 ly = mes[i].porty[1] - mes[i].porty[0] + 1;
		vector<vector<vector<u8>>> rule(li, vector<vector<u8>>(lx, vector<u8>(ly)));
		long long sum = 0;
		long long sum_end = li * lx * ly;
		vector<u32> floor;
		int j1;
		for (j1 = 0; j1 < i; j1++) {
			MES m;
			if (intersection(mes[i], mes[j1], m) != 0) {
				floor.push_back(j1+1); //记录相关的规则
				u32 mpi = (m.ip & (0xffffffff << (32 - m.Prefix))) - pi;
				u32 mli = pow(2, 32 - m.Prefix);
				u32 mpx = m.portx[0] - px;
				u32 mlx = m.portx[1] - px + 1;
				u32 mpy = m.porty[0] - py;
				u32 mly = m.porty[1] - py + 1;
				for (int z = mpi; z < mli + mpi; z++) {
					for (int x = mpx; x < mlx; x++) {
						for (int y = mpy; y < mly; y++) {
							if (rule[z][x][y] == 0) {
								rule[z][x][y] = 1;
								sum++;
							}
						}
					}
				}
				if (sum == sum_end) break; //全为1 表明已全部覆盖
				//int zz;
				//for (zz = 0; zz < li; zz++) {
				//	int xx;
				//	for (xx = 0; xx < lx; xx++) {
				//		int yy;
				//		for (yy = 0; yy < ly; yy++) {
				//			if (rule[zz][xx][yy] == 0) {
				//				break;
				//			}
				//		}
				//		if(yy != ly) break;
				//	}
				//	if (xx != lx) break;
				//}
				//if (zz == li) break; //全为1 表明已全部覆盖
			}
		}
		if (j1 != i) {  //提前结束 输出结果
			mes[i].resule = 2;
			Redu r;
			r.s = i + 1;
			cout << i+1 << ":";
			for (int j = 0; j < floor.size(); j++) {
				cout << floor[j] << " ";
				r.d.push_back(floor[j]);
			}
			redu.push_back(r);
			cout << endl;
		}
		else {
			bool flag = 0;
			int j2;
			for (j2 = i + 1; j2 < NumRule; j2++) {
				MES m5;
				if (intersection(mes[i], mes[j2], m5) == 1) {
					bool flagm = 0;
					u32 mpi = (m5.ip & (0xffffffff << (32 - m5.Prefix))) - pi;
					u32 mli = pow(2, 32 - m5.Prefix);
					u32 mpx = m5.portx[0] - px;
					u32 mlx = m5.portx[1] - px + 1;
					u32 mpy = m5.porty[0] - py;
					u32 mly = m5.porty[1] - py + 1;
					for (int z = mpi; z < mli + mpi; z++) {
						for (int x = mpx; x < mlx; x++) {
							for (int y = mpy; y < mly; y++) {
								if (rule[z][x][y] == 0) {
									rule[z][x][y] = j2 + 1;
									sum++;
									flagm = 1;
								}
							}
						}
					}
					if (flagm == 1) {
						flagm = 0;
						floor.push_back(j2 + 1); //记录相关的规则
						int aa;
						for (aa = i + 1; aa < j2; aa++) {
							MES m;
							if (intersection(mes[i], mes[aa], m) == 2) {  //异类有交集
								u32 mpi = (m.ip & (0xffffffff << (32 - m.Prefix))) - pi;
								u32 mli = pow(2, 32 - m.Prefix);
								u32 mpx = m.portx[0] - px;
								u32 mlx = m.portx[1] - px + 1;
								u32 mpy = m.porty[0] - py;
								u32 mly = m.porty[1] - py + 1;
								int zz;
								for (zz = mpi; zz < mli + mpi; zz++) {
									int xx;
									for (xx = mpx; xx < mlx; xx++) {
										int yy;
										for (yy = mpy; yy < mly; yy++) {
											if (rule[zz][xx][yy] == j2 + 1) {
												break;
											}
										}
										if (yy != mly) break;
									}
									if (xx != mlx) break;
								}
								if (zz != mli) break; //
							}
						}
						if (aa != j2) {
							flag = 0;
							break;
						}
						else {
							if (sum == sum_end) {
								flag = 1;
								break; //全为1 表明已全部覆盖
							}
							//int zz;
							//for (zz = 0; zz < li; zz++) {
							//	int xx;
							//	for (xx = 0; xx < lx; xx++) {
							//		int yy;
							//		for (yy = 0; yy < ly; yy++) {
							//			if (rule[zz][xx][yy] == 0) {
							//				break;
							//			}
							//		}
							//		if (yy != ly) break;
							//	}
							//	if (xx != lx) break;
							//}
							//if (zz == li) {  //全为1 表明已全部覆盖
							//	flag = 1;
							//	break;
							//}
						}
					}
				}
			}
			if (flag == 1) {  //提前结束 输出结果
				flag = 0;
				Redu r;
				r.s = i + 1;
				cout << i + 1 << ":";
				for (int j = 0; j < floor.size(); j++) {
					cout << floor[j] << " ";
					r.d.push_back(floor[j]);
				}
				redu.push_back(r);
				cout << endl;
			}
		}

	}
	if (redu.size() > 0) {
		for (int i = 0; i < redu.size() - 1; i++) {
			for (int j = 0; j < redu[i].d.size(); j++) {
				for (int k = i + 1; k < redu.size(); k++) {
					if (redu[i].d[j] == redu[k].s) {
						auto iter = redu[i].d.erase(std::begin(redu[i].d) + j);
						redu[i].d.insert(redu[i].d.end(), redu[k].d.begin(), redu[k].d.end());
						sort(redu[i].d.begin(), redu[i].d.end());                                             //unique只能比较相邻元素是否重复
						redu[i].d.erase(unique(redu[i].d.begin(), redu[i].d.end()), redu[i].d.end());
						sort(redu[i].d.begin(), redu[i].d.end(), less<u32>());
					}
				}
			}
		}
	}
	for (int i = 0; i < redu.size(); i++) {
		fprintf(fw,"%d:", redu[i].s);
		for (int j = 0; j < redu[i].d.size(); j++) {
			fprintf(fw, " %d", redu[i].d[j]);
		}
		fprintf(fw, "\n");
	}

	//vector<Redu> redu2;
	//Redu r1, r2;
	//r1.s = 1;
	//r1.d.push_back(2);
	//r1.d.push_back(3);
	//r1.d.push_back(4);
	//r1.d.push_back(5);
	//redu2.push_back(r1);
	//r2.s = 3;
	//r2.d.push_back(4);
	//r2.d.push_back(6);
	//r2.d.push_back(7);
	//redu2.push_back(r2);

	//for (int i = 0; i < redu2.size()-1; i++) {
	//	for (int j = 0; j < redu2[i].d.size(); j++) {
	//		for (int k = i + 1; k < redu2.size(); k++) {
	//			if (redu2[i].d[j] == redu2[k].s) {
	//				auto iter = redu2[i].d.erase(std::begin(redu2[i].d) + j);
	//				redu2[i].d.insert(redu2[i].d.end(), redu2[k].d.begin(), redu2[k].d.end());
	//				sort(redu2[i].d.begin(), redu2[i].d.end());                                             //unique只能比较相邻元素是否重复
	//				redu2[i].d.erase(unique(redu2[i].d.begin(), redu2[i].d.end()), redu2[i].d.end());
	//				sort(redu2[i].d.begin(), redu2[i].d.end(), less<u32>());
	//			}
	//		}
	//	}
	//}
	endTime = clock();
	cout << "Totle Time : " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
	fclose(fr);
	fclose(fw);
	return 0;
}
