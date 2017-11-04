// Calls FNC(type, string:typeName)
#define FORALL_ATOMICTYPES(FNC)\
	FNC(int,"int");\
	FNC(bool,"bool");\
	FNC(short,"short");\
	FNC(long,"long");\
	FNC(float,"float");\
	FNC(double,"double");\
	FNC(char,"char");

#define FORALL_ATOMICTYPES3(FNC,PARAM)\
	FNC(int,"int",PARAM);\
	FNC(bool,"bool",PARAM);\
	FNC(short,"short",PARAM);\
	FNC(long,"long",PARAM);\
	FNC(float,"float",PARAM);\
	FNC(double,"double",PARAM);\
	FNC(char,"char",PARAM);


