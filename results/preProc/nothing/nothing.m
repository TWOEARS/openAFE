clear all; close all; clc;

addpath(genpath('/home/musabini/TwoEars/AuditoryModel/TwoEars-1.2'));
startTwoEars;

load AFE_earSignals_16kHz.mat
load preProc_out.mat

requests = 'time';

dataObj = dataObject([],fsHz,10,2);

% Create a manager
mObj_DC = manager(dataObj,requests);

% Request processing
mObj_DC.processChunk(earSignals);

openAFE_left = TimeDomainSignal.construct(fsHz,10,'time','(openAFE)','left',outputSignals(:,1)); 
openAFE_right = TimeDomainSignal.construct(fsHz,10,'time','(openAFE)','right',outputSignals(:,2)); 

f1l = subplot(2,2,1);
dataObj.time{1}.plot(f1l);
f1r = subplot(2,2,2);
dataObj.time{2}.plot(f1r);
f2l = subplot(2,2,3);
openAFE_left.plot(f2l);
f2r = subplot(2,2,4);
openAFE_right.plot(f2r);
