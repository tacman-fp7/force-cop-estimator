addpath('/home/nawidj/tacman/manualOmegaCoPForceReconstruction/scripts');
expDir = '/home/nawidj/tacman/manualOmegaCoPForceReconstruction';

% Feature type list
featTypeList = {'position_tax', 'force_posTax', 'force_tax', 'activeTaxel_tax'};
featInputList = {'inputTesting','inputTesting_force','inputTesting','inputTesting'};
maxSession = 6;
minSession = 4;

% Testing data limits
xStart = 1;
xEnd = 30;


for nSession = 5%minSession:maxSession
    
    % Load the test data
    fileName = sprintf('%s/Learning_all/data/session_%02d_testing.mat',...
        expDir, nSession);
    load(fileName);
    %fileName = sprintf('%s/Learning_all/data/session_%02d_training.mat',...
    %    expDir, nSession);
    %load(fileName);
    csvwrite(sprintf('Xte_%02d.csv', nSession - minSession + 1), inputTesting(xStart:xEnd,:));
    
    for featType = 2%1:4
        % Load the position estimation data
        load(sprintf('%s/Learning_all/learnedPolicy_ANN/session%02d_ANN_type_%d.mat', expDir, nSession, 1));
        pred = net(inputTesting') * 10000;
        % Load the learned parameters
        load(sprintf('%s/Learning_all/learnedPolicy_ANN/session%02d_ANN_type_%d.mat', expDir, nSession, featType));
        inputTesting_force = [inputTesting pred'];
        
        eval(sprintf('pred = net(%s'');', featInputList{featType})); %pred = net(inputTesting');
        pred = pred';
        
        csvwrite(sprintf('yte_%s_%02d.csv', featTypeList{featType}, nSession - minSession + 1), pred(xStart:xEnd,:));
        net2file(sprintf('ANN_%s_%02d.ini', featTypeList{featType}, nSession-minSession+1), net);
        %genFunction(net, 'myNeuralNetworkFunction.m');
    end
end
