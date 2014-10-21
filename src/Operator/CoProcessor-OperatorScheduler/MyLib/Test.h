#ifndef TEST_H
#define TEST_H

void testPrimitive(int argc, char **argv);
void testJoin(int argc, char **argv);

//test the single primitive
void testReduce( int OP_rLen, int numThread, int OPERATOR );
void testFilter( int OP_rLen, int numThread );

#endif
