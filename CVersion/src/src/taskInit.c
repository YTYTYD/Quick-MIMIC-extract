#include <taskInit.h>

void task_free(struct task *target)
{
    free(target->feature_ids);
    free(target->ICD_list);
}

void task_send(int mpi_size, struct task tar)
{
    int i, j;
    for (i = 1; i < mpi_size; i++)
    {
        MPI_Send(&tar.task_id, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.feature_ids_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.ICD_list_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(tar.ICD_list, tar.ICD_list_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.age, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.gender, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.Unit1, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.Unit2, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.HospAdmTime, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.ICULOS, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.drug, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.note, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(&tar.start_time, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        for (j = 0; j < tar.feature_ids_size; j++)
        {
            MPI_Send(tar.feature_ids[j].ITEMID, 256, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(tar.feature_ids[j].name, 256, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    }
}

void task_recv(struct task *tar)
{
    MPI_Recv(&(tar->task_id), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->feature_ids_size), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->ICD_list_size), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    tar->ICD_list = (int *)malloc((tar->ICD_list_size + 1) * sizeof(int));
    memset(tar->ICD_list, -1, (tar->ICD_list_size + 1) * sizeof(int));
    MPI_Recv(tar->ICD_list, tar->ICD_list_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->age), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->gender), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->Unit1), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->Unit2), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->HospAdmTime), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->ICULOS), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->drug), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->note), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&(tar->start_time), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    tar->feature_ids = (struct Feature_ID *)malloc(tar->feature_ids_size * sizeof(struct Feature_ID));
    int i;
    for (i = 0; i < tar->feature_ids_size; i++)
    {
        memset(tar->feature_ids[i].ITEMID, -1, 256 * sizeof(int));
        MPI_Recv(tar->feature_ids[i].ITEMID, 256, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(tar->feature_ids[i].name, 256, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}