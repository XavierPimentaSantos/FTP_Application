# FTP Application

### Motivation
This application was developed in the context of the [RCOM (Computer Networks)](https://sigarra.up.pt/feup/pt/ucurr_geral.ficha_uc_view?pv_ocorrencia_id=333125) course unit, during the 3<sup>rd</sup> year of the Bachelor's Degree in Software Engineering @ FEUP, in collaboration with [Artur Telo](https://github.com/ArturTelo). We obtained a score of 16.4/20 for this project.

### The application
The application allows transfering files from remote servers using the File Transfer Protocol, authenticated with user credentials or in anonymous mode. A more detailed explanation of the application's code can be found [here](https://github.com/XavierPimentaSantos/FTP_Application/blob/main/rcom_lab2_report.pdf).

### How to use it
Open a terminal on the folder containing the code, and run `make` to compile it. To download a file, simply run `./dowload ftp://[<user>:<password>@]<host>/<url-path>`. When the execution stops, the file is presented in the same folder.
