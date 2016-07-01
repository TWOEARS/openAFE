clear all; close all; clc;

addpath(genpath('/home/musabini/TwoEars/AuditoryModel/TwoEars-1.2'));
startTwoEars;

load ../AFE_earSignals_16kHz.mat
load ild_out.mat

requests = 'ild';

dataObj = dataObject([],fsHz,10,2);

% Create a manager
mObj_DC = manager(dataObj,requests);

% Request processing
mObj_DC.processChunk(earSignals);

%%
openAFE_left = TimeFrequencySignal.construct(dataObj.ild{1}.FsHz,10,'ild','Inter Aural Level Difference (openAFE)',mObj_DC.Processors{2}.cfHz,'mono',leftOutput); 

f1l = subplot(2,1,1);
dataObj.ild{1}.plot(f1l);

f2l = subplot(2,1,2);
openAFE_left.plot(f2l);