// #include "main.h"

// int czy_w_druzynie = FALSE;
// volatile char end = FALSE;

// int rank;
// int size;

// //extern void inicjuj(int *argc, char ***argv);

// int main(int argc, char **argv)
// {
//     /* Tworzenie wątków, inicjalizacja itp */
//     MPI_Init(&argc,&argv);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &size);

//     int typ = rand() % 3 + 1;
//     if (rank == ROOT){
//         rootLoop();
//     } else if (typ == GLOWA) {
//         glowaLoop();
//     } else if (typ == TLOW) {
//         tlowLoop();
//     } else if (typ == OGON) {
//         ogonLoop();
//     }

//     //finalizuj();
//     MPI_Finalize();
//     return 0;
// }
// void rootLoop(void){
//     int zlecenieID = 0;
//     while(!end){
//         sleep(rand()%1000+1000);
//         for(int i = 1;i < size; i++)
//             MPI_Send(&zlecenieID, 1, MPI_INT, i, ZLECENIE, MPI_COMM_WORLD);
//         zlecenieID++;
//     }
// }
// void glowaLoop(void){
//     int licznikWskrzeszen = 0;
//     int tulowID, ogonID;

// }
// void tlowLoop(void){
//     int licznikWskrzeszen = 0;
//     int glowaID, ogonID;
// }
// void ogonLoop(void){
//     int licznikWskrzeszen = 0;
//     int glowaID, tulowID;
// }