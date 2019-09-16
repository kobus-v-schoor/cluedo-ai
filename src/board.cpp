#include "../include/board.h"

using namespace AI::Board;

std::vector<std::vector<int>> buildGraph();

std::vector<std::vector<int>> AI::Board::board = buildGraph();

std::vector<std::vector<int>> buildGraph()
{
    std::vector<std::vector<int>> graph;
    graph.resize(83);

    // the following code was generated using the gen_graph.py script in this directory

    //// floor horizontal connections

    // row 1
    graph[10].push_back(11);
    graph[11].push_back(10);

    graph[12].push_back(13);
    graph[13].push_back(12);
    graph[13].push_back(14);
    graph[14].push_back(13);

    // row 2
    graph[15].push_back(16);
    graph[16].push_back(15);
    graph[16].push_back(17);
    graph[17].push_back(16);
    graph[17].push_back(18);
    graph[18].push_back(17);
    graph[18].push_back(19);
    graph[19].push_back(18);
    graph[19].push_back(20);
    graph[20].push_back(19);
    graph[20].push_back(21);
    graph[21].push_back(20);
    graph[21].push_back(22);
    graph[22].push_back(21);
    graph[22].push_back(23);
    graph[23].push_back(22);
    graph[23].push_back(24);
    graph[24].push_back(23);

    // row 3
    graph[25].push_back(26);
    graph[26].push_back(25);
    graph[26].push_back(27);
    graph[27].push_back(26);
    graph[27].push_back(28);
    graph[28].push_back(27);

    graph[29].push_back(30);
    graph[30].push_back(29);
    graph[30].push_back(31);
    graph[31].push_back(30);
    graph[31].push_back(32);
    graph[32].push_back(31);

    // row 4
    graph[33].push_back(34);
    graph[34].push_back(33);
    graph[34].push_back(35);
    graph[35].push_back(34);
    graph[35].push_back(36);
    graph[36].push_back(35);

    graph[37].push_back(38);
    graph[38].push_back(37);
    graph[38].push_back(39);
    graph[39].push_back(38);
    graph[39].push_back(40);
    graph[40].push_back(39);

    // row 5
    graph[41].push_back(42);
    graph[42].push_back(41);

    graph[43].push_back(44);
    graph[44].push_back(43);
    graph[44].push_back(45);
    graph[45].push_back(44);
    graph[45].push_back(46);
    graph[46].push_back(45);

    graph[47].push_back(48);
    graph[48].push_back(47);
    graph[48].push_back(49);
    graph[49].push_back(48);
    graph[49].push_back(50);
    graph[50].push_back(49);

    // row 6
    graph[51].push_back(52);
    graph[52].push_back(51);
    graph[52].push_back(53);
    graph[53].push_back(52);
    graph[53].push_back(54);
    graph[54].push_back(53);
    graph[54].push_back(55);
    graph[55].push_back(54);
    graph[55].push_back(56);
    graph[56].push_back(55);

    graph[57].push_back(58);
    graph[58].push_back(57);
    graph[58].push_back(59);
    graph[59].push_back(58);
    graph[59].push_back(60);
    graph[60].push_back(59);

    // row 7
    graph[61].push_back(62);
    graph[62].push_back(61);
    graph[62].push_back(63);
    graph[63].push_back(62);
    graph[63].push_back(64);
    graph[64].push_back(63);
    graph[64].push_back(65);
    graph[65].push_back(64);
    graph[65].push_back(66);
    graph[66].push_back(65);
    graph[66].push_back(67);
    graph[67].push_back(66);
    graph[67].push_back(68);
    graph[68].push_back(67);
    graph[68].push_back(69);
    graph[69].push_back(68);
    graph[69].push_back(70);
    graph[70].push_back(69);
    graph[70].push_back(71);
    graph[71].push_back(70);
    graph[71].push_back(72);
    graph[72].push_back(71);

    // row 8
    graph[73].push_back(74);
    graph[74].push_back(73);

    graph[75].push_back(76);
    graph[76].push_back(75);
    graph[76].push_back(77);
    graph[77].push_back(76);
    graph[77].push_back(78);
    graph[78].push_back(77);
    graph[78].push_back(79);
    graph[79].push_back(78);
    graph[79].push_back(80);
    graph[80].push_back(79);
    graph[80].push_back(81);
    graph[81].push_back(80);
    graph[81].push_back(82);
    graph[82].push_back(81);

    //// floor vertical connections (done on row by row basis from top to bottom)

    // row 1
    graph[10].push_back(15);
    graph[15].push_back(10);
    graph[11].push_back(16);
    graph[16].push_back(11);

    graph[12].push_back(22);
    graph[22].push_back(12);
    graph[13].push_back(23);
    graph[23].push_back(13);
    graph[14].push_back(24);
    graph[24].push_back(14);

    // row 2
    graph[15].push_back(25);
    graph[25].push_back(15);
    graph[16].push_back(26);
    graph[26].push_back(16);
    graph[17].push_back(27);
    graph[27].push_back(17);
    graph[18].push_back(28);
    graph[28].push_back(18);

    graph[22].push_back(29);
    graph[29].push_back(22);
    graph[23].push_back(30);
    graph[30].push_back(23);
    graph[24].push_back(31);
    graph[31].push_back(24);

    // row 3
    graph[25].push_back(33);
    graph[33].push_back(25);
    graph[26].push_back(34);
    graph[34].push_back(26);
    graph[27].push_back(35);
    graph[35].push_back(27);
    graph[28].push_back(36);
    graph[36].push_back(28);

    graph[29].push_back(37);
    graph[37].push_back(29);
    graph[30].push_back(38);
    graph[38].push_back(30);
    graph[31].push_back(39);
    graph[39].push_back(31);
    graph[32].push_back(40);
    graph[40].push_back(32);

    // row 4
    graph[33].push_back(43);
    graph[43].push_back(33);
    graph[34].push_back(44);
    graph[44].push_back(34);
    graph[35].push_back(45);
    graph[45].push_back(35);
    graph[36].push_back(46);
    graph[46].push_back(36);

    graph[37].push_back(47);
    graph[47].push_back(37);
    graph[38].push_back(48);
    graph[48].push_back(38);
    graph[39].push_back(49);
    graph[49].push_back(39);
    graph[40].push_back(50);
    graph[50].push_back(40);

    // row 5
    graph[41].push_back(51);
    graph[51].push_back(41);
    graph[42].push_back(52);
    graph[52].push_back(42);

    graph[45].push_back(55);
    graph[55].push_back(45);
    graph[46].push_back(56);
    graph[56].push_back(46);

    graph[47].push_back(57);
    graph[57].push_back(47);
    graph[48].push_back(58);
    graph[58].push_back(48);
    graph[49].push_back(59);
    graph[59].push_back(49);
    graph[50].push_back(60);
    graph[60].push_back(50);

    // row 6
    graph[51].push_back(61);
    graph[61].push_back(51);
    graph[52].push_back(62);
    graph[62].push_back(52);
    graph[53].push_back(63);
    graph[63].push_back(53);
    graph[54].push_back(64);
    graph[64].push_back(54);
    graph[55].push_back(65);
    graph[65].push_back(55);
    graph[56].push_back(66);
    graph[66].push_back(56);

    graph[57].push_back(70);
    graph[70].push_back(57);
    graph[58].push_back(71);
    graph[71].push_back(58);
    graph[59].push_back(72);
    graph[72].push_back(59);

    // row 7
    graph[61].push_back(73);
    graph[73].push_back(61);
    graph[62].push_back(74);
    graph[74].push_back(62);

    graph[65].push_back(75);
    graph[75].push_back(65);
    graph[66].push_back(76);
    graph[76].push_back(66);
    graph[67].push_back(77);
    graph[77].push_back(67);
    graph[68].push_back(78);
    graph[78].push_back(68);
    graph[69].push_back(79);
    graph[79].push_back(69);
    graph[70].push_back(80);
    graph[80].push_back(70);
    graph[71].push_back(81);
    graph[81].push_back(71);
    graph[72].push_back(82);
    graph[82].push_back(72);

    //// room entrances/connections (not shortcuts)

    // middle room 0
    graph[0].push_back(20);
    graph[20].push_back(0);
    graph[0].push_back(37);
    graph[37].push_back(0);
    graph[0].push_back(68);
    graph[68].push_back(0);
    graph[0].push_back(36);
    graph[36].push_back(0);

    // courtyard 1
    graph[1].push_back(40);
    graph[40].push_back(1);
    graph[1].push_back(50);
    graph[50].push_back(1);

    // garage 2
    graph[2].push_back(82);
    graph[82].push_back(2);

    // games room 3
    graph[3].push_back(78);
    graph[78].push_back(3);

    // bedroom 4
    graph[4].push_back(74);
    graph[74].push_back(4);
    graph[4].push_back(5);
    graph[5].push_back(4);

    // bathroom 5
    graph[5].push_back(51);
    graph[51].push_back(5);

    // study 6
    graph[6].push_back(41);
    graph[41].push_back(6);

    // kitchen 7
    graph[7].push_back(10);
    graph[10].push_back(7);
    graph[7].push_back(8);
    graph[8].push_back(7);

    // dining room 8
    graph[8].push_back(19);
    graph[19].push_back(8);

    // living room 9
    graph[9].push_back(13);
    graph[13].push_back(9);

    //// secret passages

    // garage to kitchen
    graph[2].push_back(7);
    graph[7].push_back(2);

    // bedroom to living room
    graph[4].push_back(9);
    graph[9].push_back(4);

    return graph;
}

// vim: set expandtab textwidth=100:
