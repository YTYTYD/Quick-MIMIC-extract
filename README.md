# This paper
Quick-MIMIC: A Multimodal Data Extraction Pipeline for MIMIC with Parallelization   
Yutao Dou, Wei Li, Albert Y. Zomaya, and Shaoliang Peng  
accepted by: The 21st Asia Pacific Bioinformatics Conference  
At: Changsha, January 2023  

# Quick-MIMIC-extract
We aims to develop a reliable and easy-to-use extraction pipeline to standardize data extraction from MIMIC datasets. Our method can fully integrate different data structures, including structured, semi-structured, and unstructured data, into a time-series table. 
The format of the table is a queue with hourly granularity, one feature per column, and the format can be referred to Sepsis fromClinical Data – the PhysioNet Computing in Cardiology Challenge 2019.

## Pipeline structure
Our pipeline contains three main query modules, the hospitalization query module and the patient query module for identifying target patients in the database (de-identified IDs).
### Hospitalization Query and Patient Query Modules
The hospitalization query module and the patient query module, which are responsible for matching patient and hospitalization information in the database, ensure that patients and cases can be data pairs to generate the Subject-ID:HADM-ID structure.
Once identified, various types of information about the patient are progressively extracted and stored through the information query module.

### Information Query Module
The targets identified according to the above steps are fed into the module to query their various related information.
The module is divided into 5 components, first constructing the time architecture component and then using 4 separate extraction components to extract static features, time series features, drug information and medical notes, respectively, and rearrange them according to the time architecture.
The principle is to collect the information of specified test items for each case in each MIMIC sub-dataset and put them into the table correctly at the corresponding time. Finally the table is generated as a CSV file output for a case.

## Code
Our pipeline is available for MIMIC-III/MIMIC-IV.
_multi_extract.py(python file) are the main file used to integrate all the queried data and generate the data into CSV.
Other .sql files should be stored in the sql database to be called in the main program.


In addition, we propose a pipeline parallelization method to reduce the time of data extraction
