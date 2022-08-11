
alter PROCEDURE p_data_be_test (
 @HADM_ID varchar(10)
 ,@SUBJECT_ID  varchar(10)
) as
--set @SUBJECT_ID='36'
--set @HADM_ID='165660'

 if OBJECT_ID('tempdb.dbo.#temp') is not null
   drop table #temp
 if OBJECT_ID('tempdb.dbo.#LABEVENTS') is not null
   drop table #LABEVENTS
 if OBJECT_ID('tempdb.dbo.#CHARTEVENTS') is not null
   drop table #CHARTEVENTS
--保存LABEVENTS、CHARTEVENTS数据到临时表

 SELECT  * into #LABEVENTS
 FROM [mimic].[dbo].LABEVENTS where  [HADM_ID]=@HADM_ID 

 SELECT  * into #CHARTEVENTS from [mimic].[dbo].[CHARTEVENTS]  
where  SUBJECT_ID=@SUBJECT_ID and [HADM_ID]=@HADM_ID 

--查询时间区间
declare @begintime datetime
declare @endtime datetime
declare @begintime1 datetime
declare @endtime1 datetime
select @endtime=max( cast(CHARTTIME as datetime))
,@begintime=min(cast(CHARTTIME as datetime))
from #CHARTEVENTS  

 SELECT  @endtime1=max( cast(CHARTTIME as datetime))
 ,@begintime1=min(cast(CHARTTIME as datetime))
  FROM #LABEVENTS

  if @endtime1>@endtime
  set @endtime=@endtime1

  if @begintime1<@begintime
  set @begintime=@begintime1

--年龄
declare @DOB datetime,@age int
select @DOB=DOB from [mimic].[dbo].[PATIENTS] where SUBJECT_ID=@SUBJECT_ID

set @age=datediff(YEAR,@DOB,@begintime)
set @age=case when @age>89 then 90 else @age end
--住院时间
	declare @ADMITTIME datetime,@HospAdmTime int
	SELECT @ADMITTIME=ADMITTIME FROM [mimic].[dbo].ADMISSIONS where  [HADM_ID]=@HADM_ID

  set @HospAdmTime=DATEDIFF(HOUR,case when @ADMITTIME<@begintime then @ADMITTIME else @begintime end,@endtime)

 -- select @begintime,@endtime

--ICU
declare @Unit1 int=0
declare @Unit2 int=0
if exists(select 1 from [mimic].[dbo].[TRANSFERS] where  [HADM_ID]=@HADM_ID and	CURR_CAREUNIT ='MICU')
begin
	set @Unit1=1
end 

if exists(select 1 from [mimic].[dbo].[TRANSFERS] where  [HADM_ID]=@HADM_ID and	CURR_CAREUNIT ='SICU')
begin
	set @Unit2=1
end 
--Gender
declare @Gender int
select @Gender=case when GENDER='F' then 0 WHEN GENDER='M' THEN 1 else '' end  from [mimic].[dbo].[PATIENTS] where SUBJECT_ID=@SUBJECT_ID

  set @begintime=FORMAT(@begintime,'yyyy-MM-dd HH:00:00')
  set @endtime=FORMAT(@endtime,'yyyy-MM-dd HH:00:00')
   --select @begintime,@endtime
  declare @hour int= DATEDIFF(HOUR,@begintime,@endtime)
 declare @i int =1;
 declare @time datetime;


 create table #temp  (ICULOS int,begintime datetime,endtime datetime,HospAdmTime int,Unit1 int,Unit2 int,Gender varchar(10),Age varchar(10) )
  set @time=@begintime
  while @i<=@hour
  begin
   insert into #temp values(
   @i,@time,DATEADD(hour,1,@time),@HospAdmTime,@Unit1,@Unit2,@Gender,@age
   )
   set @i=@i+1
   set @time=DATEADD(hour,1,@time)
  end 

--select * from #temp
--SELECT * FROM (
--SELECT a.Age,a.Gender,a.Unit1,a.Unit2,a.HospAdmTime,a.ICULOS,c.Name as ITEMNAME,b.ITEMID,CAST(b.VALUENUM AS float) AS VALUENUM from #temp a left join #CHARTEVENTS b on b.CHARTTIME>=a.begintime and b.CHARTTIME<=a.endtime
--left join [dbo].[features] c on c.ITEMID=b.ITEMID 
--union all
--SELECT a.Age,a.Gender,a.Unit1,a.Unit2,a.HospAdmTime,a.ICULOS,c.Name as ITEMNAME,b.ITEMID,CAST(b.VALUENUM AS float) AS VALUENUM from #temp a left join #LABEVENTS b on b.CHARTTIME>=a.begintime and b.CHARTTIME<=a.endtime
--left join [dbo].[features] c on c.ITEMID=b.ITEMID 
--) AS T 


DECLARE @sql_str VARCHAR(8000)
DECLARE @sql_col VARCHAR(8000)
SELECT @sql_col = ISNULL(@sql_col + ',','') + QUOTENAME([NAME]) FROM [mimic].[dbo].[features] GROUP BY [NAME]
 --select  @sql_col
SET @sql_str = '
SELECT HR,	O2Sat,	Temp,	SBP,	MAP,	DBP,	Resp,	EtCO2,	BaseExcess,	HCO3,	FiO2,	pH,	PaCO2,	SaO2,	AST,	BUN,
Alkalinephos,	Calcium,	Chloride,	Creatinine,	Bilirubin_direct,	Glucose,	Lactate,	Magnesium,	Phosphate,	Potassium,	Bilirubin_total,
TroponinI,	Hct,	Hgb,	PTT,	WBC,	Fibrinogen,	Platelets,	Age,	Gender,	Unit1,	Unit2,	HospAdmTime,	ICULOS,begintime, endtime
 FROM (
    SELECT a.*,c.Name as ITEMNAME,CAST(b.VALUENUM AS float) AS VALUENUM from #temp a left join #CHARTEVENTS b on b.CHARTTIME>=a.begintime and b.CHARTTIME<=a.endtime
left join [mimic].[dbo].[features] c on c.ITEMID=b.ITEMID 
union
SELECT a.*,c.Name as ITEMNAME,CAST(b.VALUENUM AS float) AS VALUENUM from #temp a left join #LABEVENTS b on b.CHARTTIME>=a.begintime and b.CHARTTIME<=a.endtime
left join [mimic].[dbo].[features] c on c.ITEMID=b.ITEMID ) p PIVOT 
    (avg (VALUENUM) FOR [ITEMNAME] IN ( '+ @sql_col +') ) AS pvt 
ORDER BY pvt.[ICULOS]'
PRINT (@sql_str)
EXEC (@sql_str)
