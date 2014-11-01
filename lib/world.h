



#define TRUE 1
#define FALSE 0

#define BLOCK_EDGE 8
#define CONTAINER_EDGE 8
#define BLOCK_SIZE BLOCK_EDGE * BLOCK_EDGE * BLOCK_EDGE
#define CONTAINER_SIZE CONTAINER_EDGE * CONTAINER_EDGE * CONTAINER_EDGE

#define ATOM_EDGE (GLfloat) 1
#define ATOM_UNDEFINED 100
#define ATOM_EMPTY 0




typedef unsigned char Boolean;

typedef unsigned char AtomValue;

typedef struct container {
	void* aChildren[CONTAINER_SIZE];
	struct container* oParent;
	Boolean bFull, bEmpty;
	unsigned int iLevel;
	int vAbsBlockPos[3]; /// of corner
	int vPosInC[3];
} Container;

static Container oZeroContainer;

typedef struct block {
	AtomValue aAtoms[BLOCK_SIZE];
	struct container* oContainer;
	Boolean bFull, bEmpty;
	int vAbsBlockPos[3]; /// of corner
	int vPosInC[3];
	Mesh* oMesh;
} Block;

static Block oZeroBlock;




///#///

void vInitWorld () ;
Block* oNewBlock () ;
Container* oNewContainer () ;
Block* oGetBlock (int vAbsBlockPos[3]) ;
Block* oCreateBlock (int vAbsBlockPos[3]) ;
static Block* oGetOrCreateBlock (int vAbsBlockPos[3], Boolean bCreate) ;
static void vLogContainer (Container* oContainer) ;
void oMakeBlockMesh (Block* oBlock) ;
void vSetBlockAtom (Block* oBlock, int vPosInB[3], AtomValue iValue) ;
void vRefreshBlockFlags (Block* oBlock) ;
static void vRefreshContainerFlags (Container* oContainer, Boolean bNotFull, Boolean bNotEmpty) ;
Block* oGetNeighborBlock (Block* oBlock, int vDir[3]) ;
Container* oGetNeighborContainer (Container* oContainer, int vDir[3]) ;
//void vPutBlockIntoContainer (Container* oContainer, Block* oBlock, int vPosInC[3]) ;
static int iGetContainerEdgeBlockLength (Container* oContainer) ;
static int iGetContainerArrayOffset (int vPosInC[3]) ;
static int iGetBlockArrayOffset (int vPosInB[3]) ;
static void vCopyPos (int vSource[3], int vTarget[3]) ;
static int iPow (int iInt, int iPow) ;

///#///



