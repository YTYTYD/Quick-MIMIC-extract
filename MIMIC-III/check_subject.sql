



exec check_subject @HADM_ID='189535'

create proc check_subject (
@HADM_ID  varchar(10)
) 
as
SELECT  distinct [SUBJECT_ID]
  FROM [mimic].[dbo].[DIAGNOSES_ICD] where HADM_ID = @HADM_ID

--SELECT  distinct [SUBJECT_ID]
--  FROM [mimic].[dbo].[DIAGNOSES_ICD] where HADM_ID = '165660'
