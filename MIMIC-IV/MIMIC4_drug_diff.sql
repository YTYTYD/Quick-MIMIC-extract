exec MIMIC4_drug_diff @HADM_ID= 22409835

CREATE proc MIMIC4_drug_diff (
 @HADM_ID varchar(10)
) as

select  starttime,stoptime,prod_strength,gsn,ndc,drug into #drug from [mimic4].[dbo].[MIMIC4_prescriptions] where  [HADM_ID]=@HADM_ID
alter table #drug add DIFF varchar(8000)  
update #drug set DIFF = datediff(HOUR,starttime,stoptime)
select * from   #drug order by starttime