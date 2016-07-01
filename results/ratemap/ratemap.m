clear all; close all; clc;

addpath(genpath('/home/musabini/matlab_ws'));
startTwoEars;

load ../AFE_earSignals_16kHz.mat
load ratemap_out.mat

requests = 'ratemap';

dataObj = dataObject([],fsHz,10,2);

% Create a manager
mObj_DC = manager(dataObj,requests);

% Request processing
mObj_DC.processChunk(earSignals);

%%
openAFE_left = TimeFrequencySignal(mObj_DC.Processors{8},10,'left',leftOutput);
openAFE_right = TimeFrequencySignal(mObj_DC.Processors{8},10,'right',rightOutput);

f1l = subplot(2,2,1);
dataObj.ratemap{1}.plot(f1l);
f1r = subplot(2,2,2);
dataObj.ratemap{2}.plot(f1r);
f2l = subplot(2,2,3);
openAFE_left.plot(f2l);
title('Ratemap extraction - left (openAFE)');
f2r = subplot(2,2,4);
openAFE_right.plot(f2r);
title('Ratemap extraction - right (openAFE)');
