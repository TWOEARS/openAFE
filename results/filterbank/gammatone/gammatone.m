clear all; close all; clc;

addpath(genpath('/home/musabini/matlab_ws'));
startTwoEars;

load AFE_earSignals_16kHz.mat
%load gammatone_out.mat
load /home/musabini/externalLibs/exampleAutconf/openAFE/build/examples/gtt.mat

requests = 'filterbank';

dataObj = dataObject([],fsHz,10,2);

% Create a manager
mObj_DC = manager(dataObj,requests);

% Request processing
mObj_DC.processChunk(earSignals);

%%
openAFE_left = TimeFrequencySignal.construct(fsHz,10,'filterbank','Gammatone (openAFE)',mObj_DC.Processors{2}.cfHz,'left',leftOutput); 
openAFE_right = TimeFrequencySignal.construct(fsHz,10,'filterbank','Gammatone (openAFE)',mObj_DC.Processors{2}.cfHz,'right',rightOutput); 

f1l = subplot(2,2,1);
dataObj.filterbank{1}.plot(f1l);
f1r = subplot(2,2,2);
dataObj.filterbank{2}.plot(f1r);
f2l = subplot(2,2,3);
openAFE_left.plot(f2l);
f2r = subplot(2,2,4);
openAFE_right.plot(f2r);