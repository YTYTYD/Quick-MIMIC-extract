alter proc extract1 (
 @HADM_ID varchar(10)
) as


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
where  [HADM_ID]=@HADM_ID 

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

  set @begintime=FORMAT(@begintime,'yyyy-MM-dd HH:00:00')
  set @endtime=FORMAT(@endtime,'yyyy-MM-dd HH:00:00')


 
 declare @hour int= DATEDIFF(HOUR,@begintime,@endtime)
 declare @i int =1;
 declare @time datetime;


 create table #temp  (ICULOS int,begintime datetime,endtime datetime )
  set @time=@begintime
  while @i<=@hour
  begin
   insert into #temp values(
   @i,@time,DATEADD(hour,1,@time)
   )
   set @i=@i+1
   set @time=DATEADD(hour,1,@time)
  end 

  
 --text  #texttable1
select  TEXT,CHARTDATE  into #texttable1 from [mimic].[dbo].[NOTEEVENTS] where  [HADM_ID]=@HADM_ID

update #texttable1
set CHARTDATE =FORMAT(cast(CHARTDATE as datetime),'yyyy-MM-dd 00:00:00')

SELECT  CHARTDATE, TEXT = STUFF((select';'+ cast(TEXT as varchar(8000))  from #texttable1  where  CHARTDATE = d.CHARTDATE for xml path('')),1,1,'')  into #textex
FROM #texttable1 d group by CHARTDATE 


select #textex.TEXT from #temp left join #textex on #textex.CHARTDATE = #temp.endtime order by ICULOS


