#define NumStages_Mark 2
#define NumClassifiers_Mark 3
#define NumParas_Mark 64

double Mark_CascadeParaTable[NumParas_Mark] = {40, 25, 2, 1, 1, 3, 9, 3, 6, 16, -1, 9, 3, 3, 8, 2, 12, 11, 3, 8, 2, 0, 0.047854, -1.000000, 0.994061, 0.994061, -1, -1, 2, 1, 2, 4, 0, 28, 20, -1, 4, 5, 28, 10, 2, 0, 0.252720, -1.000000, 0.996030, 1, 2, 16, 10, 8, 8, -1, 16, 14, 8, 4, 2, 0, -0.042733, 0.992085, -0.999971, 1.988115, 0, -1};
CtHaarClassifierCascade MarkCascade;
CtHaarStageClassifier MarkCascadeStages[NumStages_Mark];
CtHaarClassifier MarkCascadeClassifiers[NumClassifiers_Mark];