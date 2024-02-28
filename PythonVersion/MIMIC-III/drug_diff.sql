exec drug_diff @HADM_ID= 107064

alter proc drug_diff (
 @HADM_ID varchar(10)
) as

select  STARTDATE,ENDDATE,PROD_STRENGTH,GSN,NDC into #drug from [mimic].[dbo].[PRESCRIPTIONS] where  [HADM_ID]=@HADM_ID
alter table #drug add DIFF varchar(8000)  
update #drug set DIFF = datediff(HOUR,STARTDATE,ENDDATE)
select * from   #drug order by STARTDATE