struct index_data *PRESCRIPTION_M_INDEX;
#ifdef MIMICIV
struct index_64_data *LABEVENT_M_INDEX;
#elif defined(MIMICIII)
struct index_data *LABEVENT_M_INDEX;
struct index_data *NOTEEVENT_M_INDEX;
#endif

void index_read()
{
    read_index_file(PRESCRIPTION_INDEX, &PRESCRIPTION_M_INDEX, PRESCRIPTION_LEN);
#ifdef MIMICIV
    read_index_file_64(LABEVENTS_INDEX, &LABEVENT_M_INDEX, LABEVENTS_LEN);
#elif defined(MIMICIII)
    read_index_file(LABEVENTS_INDEX, &LABEVENT_M_INDEX, LABEVENTS_LEN);
    read_index_file(NOTEEVENT_INDEX, &NOTEEVENT_M_INDEX, NOTEEVENT_LEN);
#endif
}