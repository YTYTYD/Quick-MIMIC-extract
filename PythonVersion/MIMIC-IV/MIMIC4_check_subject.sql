exec MIMIC4_check_subject @hadm_id='20475282'

create proc MIMIC4_check_subject (
@hadm_id  varchar(10)
) 
as
SELECT  distinct [subject_id]
  FROM [mimic4].[dbo].[MIMIC4_diagnoses_icd] where hadm_id = @hadm_id
