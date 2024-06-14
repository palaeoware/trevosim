#include "testinternal.h"
#include "simulation.h"

#include <QCryptographicHash>

testinternal::testinternal(MainWindow *theMainWindowCon)
{
    theMainWindow = theMainWindowCon;
    error = false;

    //This is currently used (June 2024) to return the test description
    //Long term it would be good to create a test class for each test which returns a description and includes the test code
    testList.insert(0, "Fitness Algorithm");
    testList.insert(1, "Mask initialisation");
    testList.insert(2, "Masks during simulation");
    testList.insert(3, "Initialisation of playing field");
    testList.insert(4, "Stochastic mapping");
    testList.insert(5, "Mutation rates");
    testList.insert(6, "Coin toss");
    testList.insert(7, "Speciation");
    testList.insert(8, "Overwrite");
    testList.insert(9, "Perturbations");
    testList.insert(10, "Strip uninformative");
    testList.insert(11, "Unresolvable taxa");
    testList.insert(12, "Memory use");
    testList.insert(13, "Extinction");
    testList.insert(14, "Difference to parent");
    testList.insert(15, "Print matrix");
    testList.insert(16, "Print tree");
    testList.insert(17, "Ecosystem engineers");
    testList.insert(18, "Playing field mixing");
}

QString testinternal::testDescription(int testNumber)
{
    if (testList.contains(testNumber)) return testList.value(testNumber);
    else return "No description available";
}

bool testinternal::callTest(int testNumber, QString &outString)
{
    bool pass;
    switch (testNumber)
    {
    case 0:
        pass = testZero(outString);
        return pass;
    case 1:
        pass = testOne(outString);
        return pass;
    case 2:
        pass = testTwo(outString);
        return pass;
    case 3:
        pass = testThree(outString);
        return pass;
    case 4:
        pass = testFour(outString);
        return pass;
    case 5:
        pass = testFive(outString);
        return pass;
    case 6:
        pass = testSix(outString);
        return pass;
    case 7:
        pass = testSeven(outString);
        return pass;
    case 8:
        pass = testEight(outString);
        return pass;
    case 9:
        pass = testNine(outString);
        return pass;
    case 10:
        pass = testTen(outString);
        return pass;
    case 11:
        pass = testEleven(outString);
        return pass;
    case 12:
        pass = testTwelve(outString);
        return pass;
    case 13:
        pass = testThirteen(outString);
        return pass;
    case 14:
        pass = testFourteen(outString);
        return pass;
    case 15:
        pass = testFifteen(outString);
        return pass;
    case 16:
        pass = testSixteen(outString);
        return pass;
    case 17:
        pass = testSeventeen(outString);
        return pass;
    case 18:
        pass = testEighteen(outString);
        return pass;
    case 19:
        pass = testNineteen(outString);
        return pass;
    case 20:
        pass = testTwenty(outString);
        return pass;
    }
    return false;
}

//Test fitness algorithm - send fitness function known masks and organism, and check output
bool testinternal::testZero(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Testing fitness algorithm.\n\n";

    //Create default setting object and then a simulation object for the test
    simulationVariables simSettings;
    simSettings.genomeSize = 50;
    simSettings.fitnessSize = 50;
    simSettings.speciesSelectSize = 50;
    //Test is for three masks and a target of zero (the defaults in v2.0.0)
    simSettings.fitnessTarget = 0;
    simSettings.maskNumber = 3;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) testFlag = false;

    //Fitness requires an organism - create an organism with 50 bits, no stochastic genome, all bits are initialised to zero
    Organism org(simSettings.genomeSize, false);
    out << "Organism genome is: " << x.printGenomeString(&org) << "\n";

    //Now set masks in simulation to 1
    for (auto p : qAsConst(x.playingFields))
        for (int k = 0; k < simSettings.environmentNumber; k++)
            for (int j = 0; j < simSettings.maskNumber; j++)
                for (auto &i : p->masks[k][j]) i = true;

    QString maskString = x.printMasks(x.playingFields);
    QStringList l = maskString .split('\n');
    out << "Masks are:\n" << l[2]  << "\n" << l[3] << "\n" << l[4] << "\n";

    int fitness = x.fitness(&org, x.playingFields[0]->masks, simSettings.fitnessSize, simSettings.fitnessTarget, simSettings.maskNumber);
    if (fitness != 150) testFlag = false;
    out <<  "Fitness, with fitness target of " << simSettings.fitnessTarget << " is " << fitness << ". It should be 150.\n";

    simSettings.fitnessTarget = 75;
    fitness = x.fitness(&org, x.playingFields[0]->masks, simSettings.fitnessSize, simSettings.fitnessTarget, simSettings.maskNumber);
    if (fitness != 75) testFlag = false;
    out <<  "Fitness, with fitness target of 75, is " << fitness << ". It should be 75.\n";

    for (int i = 0; i < 25; i++)org.genome[i] = true;
    out << "Fitness target is still 75, genome is now: " << x.printGenomeString(&org) << "\n";
    fitness = x.fitness(&org, x.playingFields[0]->masks, simSettings.fitnessSize, simSettings.fitnessTarget, simSettings.maskNumber);
    if (fitness != 0) testFlag = false;
    out <<  "Fitness is " << fitness << ". It should be 0.\n";

    simSettings.maskNumber = 2;
    simSettings.fitnessTarget = 50;
    out << "Set masks to two, and fitness target back to 50.\n";
    simulation y(0, &simSettings, &error, theMainWindow);
    if (error) testFlag = false;

    //Now set masks in simulation to 1
    for (auto p : qAsConst(y.playingFields))
        for (int k = 0; k < simSettings.environmentNumber; k++)
            for (int j = 0; j < simSettings.maskNumber; j++)
                for (auto &i : p->masks[k][j]) i = true;

    for (int i = 0; i < simSettings.fitnessSize; i++)org.genome[i] = true;
    maskString = y.printMasks(y.playingFields);
    l = maskString .split('\n');
    out << "Organism genome is: " << y.printGenomeString(&org) << "\n";
    out << "Masks are:\n" << l[2]  << "\n" << l[3] << "\n";
    fitness = y.fitness(&org, y.playingFields[0]->masks, simSettings.fitnessSize, simSettings.fitnessTarget, simSettings.maskNumber);
    if (fitness != 50) testFlag = false;
    out <<  "Fitness is " << fitness << ". It should be 50.\n\n";

    //Now let's test with two environments
    out << "Two environments, one all ones, the other all zeros, target is zero.\n";

    simSettings.environmentNumber = 2;
    simSettings.fitnessTarget = 0;
    simulation z(0, &simSettings, &error, theMainWindow);
    if (error) testFlag = false;

    //Now set masks in environment 0 simulation to 1
    for (auto p : qAsConst(z.playingFields))
        for (int j = 0; j < simSettings.maskNumber; j++)
            for (auto &i : p->masks[0][j]) i = true;

    //Now set masks in environment 1 simulation to 0
    for (auto p : qAsConst(z.playingFields))
        for (int j = 0; j < simSettings.maskNumber; j++)
            for (auto &i : p->masks[1][j]) i = false;

    for (int i = 0; i < 10; i++)org.genome[i] = false;

    maskString = z.printMasks(z.playingFields);
    l = maskString.split('\n');
    out << "Organism genome is: " << y.printGenomeString(&org) << " fitness target is " << simSettings.fitnessTarget << "\n";
    out << maskString;

    int environmentZeroFitness = z.fitness(&org, z.playingFields[0]->masks, simSettings.fitnessSize, simSettings.fitnessTarget, simSettings.maskNumber, 0);
    out << "Environment zero fitness should be 20, it is " <<  environmentZeroFitness << ".\n";
    if (environmentZeroFitness != 20) testFlag = false;

    int environmentOneFitness = z.fitness(&org, z.playingFields[0]->masks, simSettings.fitnessSize, simSettings.fitnessTarget, simSettings.maskNumber, 1);
    out << "Environment one fitness should be 80, it is " <<  environmentOneFitness << ".\n";
    if (environmentOneFitness != 80) testFlag = false;
    if (environmentOneFitness == environmentZeroFitness) testFlag = false;

    int environmentBothFitness = z.fitness(&org, z.playingFields[0]->masks, simSettings.fitnessSize, simSettings.fitnessTarget, simSettings.maskNumber);
    out << "Fitness based on both environments should be 20, it is " <<  environmentBothFitness << ".\n";
    if (environmentBothFitness != 20 || environmentBothFitness != environmentZeroFitness) testFlag = false;

    if (testFlag) out << "\nFitness tests passed.\n";

    return testFlag;
}

//Test mask initialisation, and behaviour with multiple playing fields in different mask modes
bool testinternal::testOne(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Mask mode at initialisation. If there is more than one playing field this will set the  playing field masks/environments to be the same or different as the mode dictates.\n";

    //Create default setting sonject and then a simulation object for the test
    simulationVariables simSettings;
    simSettings.genomeSize = 50;
    simSettings.fitnessSize = 50;
    simSettings.speciesSelectSize = 50;

    //Mode identical
    simSettings.playingfieldMasksMode = MASKS_MODE_IDENTICAL;
    simSettings.playingfieldNumber = 3;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    if (x.playingFields[0]->masks != x.playingFields[1]->masks || x.playingFields[1]->masks != x.playingFields[2]->masks) testFlag = false;
    QString flagString = testFlag ? "true" : "false";

    QString masks(x.printMasks(x.playingFields, 1));
    QString masks2(x.printMasks(x.playingFields, 2));

    out << "\nMode identical:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are identical (they should be) and outputs " << flagString << ".\n";

    //Mode independent
    simSettings.playingfieldMasksMode = MASKS_MODE_INDEPENDENT;
    simulation y(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    if (y.playingFields[0]->masks == y.playingFields[1]->masks || y.playingFields[1]->masks == y.playingFields[2]->masks) testFlag = false;
    flagString = testFlag ? "true" : "false";

    masks = (y.printMasks(y.playingFields, 1));
    masks2 = (y.printMasks(y.playingFields, 2));

    out << "\nMode independent:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are not identical and outputs " << flagString  << ".\n";

    //Mode identical at start
    simSettings.playingfieldMasksMode = MASKS_MODE_IDENTICAL_START;
    simulation z(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    if (z.playingFields[0]->masks != z.playingFields[1]->masks || z.playingFields[1]->masks != z.playingFields[2]->masks) testFlag = false;
    flagString = testFlag ? "true" : "false";

    masks = (z.printMasks(z.playingFields, 1));
    masks2 = (z.printMasks(z.playingFields, 2));

    out << "\nMode start identical:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are identical and outputs " << flagString << ".\n";

    if (testFlag) out << "\nMask initiation tests passed.\n";

    return testFlag;
}

//Test masks in different playing fields for different modes after a simulation has been running
bool testinternal::testTwo(QString &outString)
{

    bool testFlag = true;
    QTextStream out(&outString);
    out << "Masks after 100 iterations. This will differ based on mode.\n\n";

    //Create default setting object and then a simulation object for the test
    simulationVariables simSettings;
    simSettings.genomeSize = 50;
    simSettings.fitnessSize = 50;
    simSettings.speciesSelectSize = 50;
    simSettings.playingfieldNumber = 3;
    simSettings.runForTaxa = 5;
    simSettings.test = 2;

    if (theMainWindow)
        theMainWindow->resizeGrid(simSettings.runForTaxa, simSettings.genomeSize);

    //Mode identical
    simSettings.playingfieldMasksMode = MASKS_MODE_IDENTICAL;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    x.run();
    if (x.playingFields[0]->masks != x.playingFields[1]->masks || x.playingFields[1]->masks != x.playingFields[2]->masks) testFlag = false;
    QString flagString = testFlag ? "true" : "false";

    QString masks(x.printMasks(x.playingFields, 1));
    QString masks2(x.printMasks(x.playingFields, 2));

    out << "Mode identical:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are identical and outputs " << flagString << ".\n";

    //Mode independent
    simSettings.playingfieldMasksMode = MASKS_MODE_INDEPENDENT;
    simulation y(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    y.run();
    if (y.playingFields[0]->masks == y.playingFields[1]->masks || y.playingFields[1]->masks == y.playingFields[2]->masks) testFlag = false;
    flagString = testFlag ? "true" : "false";

    masks = (y.printMasks(y.playingFields, 1));
    masks2 = (y.printMasks(y.playingFields, 2));

    out << "\nMode independent:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are not identical and outputs " << flagString  << ".\n";


    //Mode identical at start
    simSettings.playingfieldMasksMode = MASKS_MODE_IDENTICAL_START;
    simulation z(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    z.run();
    if (z.playingFields[0]->masks == z.playingFields[1]->masks || z.playingFields[1]->masks == z.playingFields[2]->masks) testFlag = false;
    flagString = testFlag ? "true" : "false";

    masks = (z.printMasks(z.playingFields, 1));
    masks2 = (z.printMasks(z.playingFields, 2));

    out << "\nMode start identical:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are not identical and outputs " << flagString << ".\n";

    if (testFlag) out << "\nMask mode tests passed.\n";

    simulationVariables simSettingsReset;
    if (theMainWindow)
    {
        theMainWindow->resizeGrid(simSettingsReset.runForTaxa, simSettingsReset.genomeSize);
        theMainWindow->resetDisplays();
    }
    return testFlag;
}

//Test initialisation of playing fields - should be populated with identical individual
bool testinternal::testThree(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Checking that all organisms are identical after initialisation in all mask modes.\n\n";

    simulationVariables simSettings;
    simSettings.genomeSize = 50;
    simSettings.fitnessSize = 50;
    simSettings.speciesSelectSize = 50;
    simSettings.playingfieldNumber = 3;
    simSettings.runForTaxa = 5;
    simSettings.test = 3;
    simSettings.workingLog = true;

    if (theMainWindow)
        theMainWindow->resizeGrid(simSettings.runForTaxa, simSettings.genomeSize);

    //Mode identical
    simSettings.playingfieldMasksMode = MASKS_MODE_IDENTICAL;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    x.run();

    Organism org(*x.playingFields[0]->playingField[0]);
    for (auto p : qAsConst(x.playingFields))
        for (auto o : qAsConst(p->playingField))
            if (!(*o == org)) testFlag = false;

    QString flagString = testFlag ? "true" : "false";
    out << "Mode identical - checked if all organisms are the same after initialisation and returned " << flagString << ".\n";

    //Mode independent
    simSettings.playingfieldMasksMode = MASKS_MODE_INDEPENDENT;
    simulation y(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    y.run();

    org = *y.playingFields[0]->playingField[0];
    for (auto p : qAsConst(y.playingFields))
        for (auto o : qAsConst(p->playingField))
            if (!(*o == org)) testFlag = false;
    flagString = testFlag ? "true" : "false";
    out << "Mode independent - checked if all organisms are the same after initialisation and returned " << flagString << ".\n";

    //Mode identical at start
    simSettings.playingfieldMasksMode = MASKS_MODE_IDENTICAL_START;
    simulation z(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    z.run();
    org = *z.playingFields[0]->playingField[0];
    for (auto p : qAsConst(z.playingFields))
        for (auto o : qAsConst(p->playingField))
            if (!(*o == org)) testFlag = false;
    flagString = testFlag ? "true" : "false";
    out << "Mode identical at start - checked if all organisms are the same after initialisation and returned " << flagString << ".\n";

    if (testFlag) out << "\nInitialisation tests passed.\n";

    simulationVariables simSettingsReset;
    if (theMainWindow)
    {
        theMainWindow->resizeGrid(simSettingsReset.runForTaxa, simSettingsReset.genomeSize);
        theMainWindow->resetDisplays();
    }

    return testFlag;
}

//Test stochastic mapping - creation of genome from stochastic layer using user defined map
bool testinternal::testFour(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    simulationVariables simSettings;
    simSettings.stochasticLayer = true;
    for (auto &i : simSettings.stochasticMap) i = 0;

    out << "Testing stochastic mapping.\n\nStochastic map is currently:\n";
    for (auto i : simSettings.stochasticMap) i ? out << "1" : out << "0";
    Organism org(20, true);
    org.initialise(20, simSettings.stochasticMap);
    out << "\nStochastic genome is: ";
    for (auto i : qAsConst(org.stochasticGenome)) i ? out << "1" : out << "0";
    out << "\nGenome is: ";
    for (auto i : qAsConst(org.genome)) i ? out << "1" : out << "0";
    for (auto i : qAsConst(org.genome)) if (i) testFlag = false;

    out << "\nStochastic map has been updated and is now:\n";
    for (auto &i : simSettings.stochasticMap) i = 1;
    for (auto i : simSettings.stochasticMap) i ? out << "1" : out << "0";
    org.initialise(20, simSettings.stochasticMap);
    out << "\nStochastic genome is: ";
    for (auto i : qAsConst(org.stochasticGenome)) i ? out << "1" : out << "0";
    out << "\nGenome is: ";
    for (auto i : qAsConst(org.genome)) i ? out << "1" : out << "0";
    for (auto i : qAsConst(org.genome)) if (!i) testFlag = false;

    QString flagString = testFlag ? "true" : "false";
    out << "\n\nGiven the mapping the first genome should be all zeros, no matter what the stochastic genome, and the second all ones. TRevoSIm tested this and returned " << flagString << ".";

    if (testFlag) out << "\n\nStochastic mapping tests passed.\n";
    return testFlag;
}

//Test mutation rates for environment and organism
bool testinternal::testFive(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Testing mutation rates.\n\n";

    if (theMainWindow)
    {
        theMainWindow->addProgressBar(0, 10000);
        theMainWindow->setStatus("Doing organism mutation tests");
    }

    simulationVariables simSettings;
    simSettings.organismMutationRate = 1.;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;

    Organism org1(simSettings.genomeSize, false);
    int cnt = 0;

    for (int j = 0; j < 10000; j++)
    {
        if (theMainWindow)
            theMainWindow->setProgressBar(j);
        org1.initialise(simSettings.genomeSize);
        Organism org2(org1);
        x.mutateOrganism(org1, x.playingFields[0]);

        for (int i = 0; i < org1.genome.length(); i++)if (org1.genome[i] != org2.genome[i]) cnt++;
    }

    double mean = static_cast<double>(cnt) / 10000;
    QString flagString = testFlag ? "true" : "false";
    out << "Ran 10000 mutations of a 128 bit organism. At a rate of " << simSettings.organismMutationRate << "mutation per hundred characters per iteration this resulted in a mean of ";
    out << mean << " mutations. TREvoSim expects this to be between 1.25 and 1.31 and returned " << flagString << "\n";
    if (mean < 1.25 || mean > 1.31) testFlag = false;

    simSettings.environmentNumber = 2;
    simSettings.playingfieldNumber = 2;
    simSettings.playingfieldMasksMode = MASKS_MODE_IDENTICAL_START;
    simulation y(0, &simSettings, &error, theMainWindow);
    if (error) return false;

    QVector <QVector <QVector <bool> > > masks;
    QVector <QVector <QVector <bool> > > masks2;
    int cnts[12] = {0};

    if (theMainWindow)
        theMainWindow->setStatus("Doing environment mutation tests");

    for (int i = 0; i < 10000; i++)
    {
        if (theMainWindow)
            theMainWindow->setProgressBar(i);
        masks = y.playingFields[0]->masks;
        masks2 =  y.playingFields[1]->masks;
        y.mutateEnvironment();

        for (int k = 0; k < masks[0][0].length(); k++)
        {
            for (int j = 0; j < 3; j++) if (y.playingFields[0]->masks[0][j][k] != masks[0][j][k])cnts[j]++;
            for (int j = 0; j < 3; j++) if (y.playingFields[0]->masks[1][j][k] != masks[1][j][k])cnts[j + 3]++;
            for (int j = 0; j < 3; j++) if (y.playingFields[1]->masks[0][j][k] != masks2[0][j][k])cnts[j + 6]++;
            for (int j = 0; j < 3; j++) if (y.playingFields[1]->masks[1][j][k] != masks2[1][j][k])cnts[j + 9]++;

        }
    }

    if (theMainWindow)
        theMainWindow->hideProgressBar();

    out << "Now testing environment mutation across two playing fields (mode independent), and two environments for each. Same test for each as above. \nPlaying field 1:\nEnvironment 1:\t";

    double dCnts[12] = {0.};
    for (int i = 0; i < 12; i++)
    {
        dCnts[i] = (static_cast<double>(cnts[i]) / 10000.);
        if (dCnts[i] < 1.25 || dCnts[i] > 1.31) testFlag = false;

        if (i == 3) out << "Environment 2: ";
        if (i == 6) out << "Playing field 2:\nEnvironment 1: ";
        if (i == 9) out << "Environment 2: ";

        out << dCnts[i] << "\t";

        if ((i + 1) % 3 == 0) out << "\n";
    }

    flagString = testFlag ? "true" : "false";

    out << "TREvoSim expects all above to be between 1.25 and 1.31 and returned " << flagString << "\n";

    if (testFlag) out << "\nMutation tests passed.\n";

    return testFlag;
}

//Test the coin toss
bool testinternal::testSix(QString &outString)
{

    if (theMainWindow)
    {
        theMainWindow->addProgressBar(0, 100000);
        theMainWindow->setStatus("Testing coin toss.\n");
    }

    bool testFlag = true;
    QTextStream out(&outString);
    out << "Testing coin toss.\n\n";

    simulationVariables simSettings;
    simSettings.selectionCoinToss = 2.;
    simSettings.playingfieldNumber = 4;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;

    int pfSize = x.playingFields[0]->playingField.length();

    for (int i = 0; i < pfSize; i++)
    {
        x.playingFields[0]->playingField[i]->fitness = i;
        x.playingFields[1]->playingField[i]->fitness = 1;
        x.playingFields[2]->playingField[i]->fitness = i % 2;
        x.playingFields[3]->playingField[i]->fitness = i % 3;
    }

    QVector<int> hits0(pfSize, 0);
    QVector<int> hits1(pfSize, 0);
    QVector<int> hits2(pfSize, 0);
    QVector<int> hits3(pfSize, 0);
    QVector<int> hits4(pfSize, 0);

    for (int i = 0; i < 100000; i++)
    {
        if (theMainWindow)
            theMainWindow->setProgressBar(i);
        hits0[x.coinToss(x.playingFields[0])]++;
        hits1[x.coinToss(x.playingFields[1])]++;
        hits2[x.coinToss(x.playingFields[2])]++;
        hits3[x.coinToss(x.playingFields[3])]++;
    }

    //Try also with a different probability
    simSettings.selectionCoinToss = 3.0;
    for (int i = 0; i < 100000; i++)hits4[x.coinToss(x.playingFields[0])]++;

    out << "<table><tr><th>Position</th><th>"
        "</th><th>Fittest at top - selection 2</th><th>"
        "</th><th>Fittest at top - selection 3</th><th>"
        "</th><th>Equal Fitness</th><th>"
        "</th><th>Alternating fitness</th><th>"
        "</th><th>Three fitnesses</th></tr>";

    for (int i = 0; i < pfSize; i++)
    {
        out << "<tr><td>" << i << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits0[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits4[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits1[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits2[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits3[i] << "</tr>";
    }
    out << "</table>\nRunning a bunch of tests comparing expected distribution to actual. Those towards bottom of playing field are based on small numbers, so maybe be printed below, but tests fail only when <em>n</em> is high enough.\n";
    for (int i = 0; i < pfSize - 1; i++)
    {
        double divisor = static_cast<double>(hits0[i]) / static_cast<double>(hits0[i + 1]);
        if (divisor < 1.8 || divisor > 2.2)
        {
            //Only fail if this happens in the first five - with small counts at the end of the playing field there is more variability
            if (i < 5)testFlag = false;
            out << "Position: " << i << "; Test 1, divisor: " <<  divisor << "\n";
        }

        divisor = static_cast<double>(hits4[i]) / static_cast<double>(hits4[i + 1]);
        if (divisor < 1.3 || divisor > 2.7)
        {
            //Only fail if this happens in the first five - with small counts at the end of the playing field there is more variability
            if (i < 5)testFlag = false;
            out << "Position: " << i << "; Test 4, divisor: " <<  divisor << "\n";
        }

        divisor = static_cast<double>(hits1[i]) / static_cast<double>(hits1[i + 1]);
        if (divisor < 0.9 || divisor > 1.1)
        {
            testFlag = false;
            out << "Test 2, divisor: " <<  divisor << "\n";
        }
    }

    int cnt = 0, cnt2 = 0;
    for (int i = 0; i < pfSize / 2; i++) cnt += hits0[i];
    for (int i = 0; i < pfSize; i++)
        if (i % 2 == 0)cnt2 += hits2[i];

    double divisor = static_cast<double>(cnt) / static_cast<double>(cnt2);
    if (divisor < 0.9 || divisor > 1.1)
    {
        testFlag = false;
        out << "Test 3, divisor: " <<  divisor << "\n";
    }

    out << "\nTested four playing fields, with decreasing fitness, equal fitness, and two or three values any failures to test printed above.\n";
    if (testFlag) out << "\nCoin toss tests passed.\n";

    if (theMainWindow)
        theMainWindow->hideProgressBar();

    return testFlag;
}

//Test the new species function - called when SD exceeded
bool testinternal::testSeven(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing new species - new species created at iteration 66 with a genome of all 1's.\n\n";

    simulationVariables simSettings;
    simSettings.genomeSize = 50;
    simSettings.fitnessSize = 50;
    simSettings.speciesSelectSize = 50;

    simulation x(0, &simSettings, &error, theMainWindow);

    if (error)
    {
        out << "Error initialising test simulation";
        return false;
    }
    //This should update the counters on new species
    x.iterations = 66;
    x.speciesCount = 14;

    //This will have initialised values - i.e. zero for everything except the genome, which will be 50% 1's
    Organism newSpecies(50, false);
    newSpecies.initialise(50);
    newSpecies.speciesID = 10;
    for (auto &i : newSpecies.genome) i = true;

    Organism parentSpecies(50, false);
    for (auto &i : parentSpecies.genome)i = true;
    for (auto &i : parentSpecies.parentGenome)i = true;
    parentSpecies.speciesID = 10;
    parentSpecies.parentSpeciesID = 9;
    parentSpecies.born = 15;
    parentSpecies.extinct = 20;
    parentSpecies.cladogenesis = 25;
    parentSpecies.fitness = 5;
    x.playingFields[0]->playingField[8]->speciesID = 10;

    x.newSpecies(newSpecies, parentSpecies, x.playingFields[0]);

    //Ok, so first, newspecies should be born iteration 66
    if (newSpecies.born != 66)testFlag = false;
    out << "New species born at iteration " << newSpecies.born << " (should be 66).\n";
    if (x.speciesCount != 15)testFlag = false;
    out << "Species number " << x.speciesCount << " (should be 15).\n";
    if (newSpecies.speciesID != 15)testFlag = false;
    out << "New species is " << newSpecies.speciesID << " (should be 15).\n";
    if (newSpecies.cladogenesis != 66)testFlag = false;
    out << "New species cladogenesis at iteration " << newSpecies.cladogenesis << " (should be 66).\n";
    if (parentSpecies.cladogenesis != 66)testFlag = false;
    out << "Parent cladogenesis at iteration " << parentSpecies.cladogenesis << " (should be 66).\n";
    if (newSpecies.parentSpeciesID != 10)testFlag = false;
    out << "New species parent species ID " << newSpecies.parentSpeciesID << " (should be 10).\n";
    for (auto i : qAsConst(newSpecies.parentGenome)) if (i != 1)testFlag = false;
    out << "New species parent genome: ";
    for (auto i : qAsConst(newSpecies.parentGenome)) i ? out << "1" : out << "0";
    out << " (should be all 1s).\n";
    for (auto i : qAsConst(newSpecies.genome)) if (i != 1)testFlag = false;
    out << "New species genome: ";
    for (auto i : qAsConst(newSpecies.genome)) i ? out << "1" : out << "0";
    out << " (should be all 1s).\n";
    for (auto i : qAsConst(x.playingFields[0]->playingField[8]->parentGenome)) if (i != 1)testFlag = false;
    out << "Species 10 parent genome in playing field is now: ";
    for (auto i :  qAsConst(x.playingFields[0]->playingField[8]->parentGenome)) i ? out << "1" : out << "0";
    out << " (should be all 1s).\n";

    if (testFlag) out << "\nNew species tests passed.\n";

    return testFlag;
}

//Test which organism to overwrite in playing field
bool testinternal::testEight(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing overwrite code.\n\n";

    simulationVariables simSettings;

    simSettings.randomOverwrite = false;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    for (auto &o : x.playingFields[0]->playingField) o->fitness = 0;
    int pfSize = x.playingFields[0]->playingField.length();
    simulation y(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    for (int i = 0; i < pfSize ; i++) y.playingFields[0]->playingField[i]->fitness = i;
    simulation z(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    for (int i = 0; i < pfSize ; i++)
        if (i > pfSize / 2) z.playingFields[0]->playingField[i]->fitness = 0;
        else z.playingFields[0]->playingField[i]->fitness = 1;

    simulationVariables simSettings2;
    simSettings2.randomOverwrite = true;
    simulation a(0, &simSettings2, &error, theMainWindow);
    if (error) return false;
    for (auto &o : a.playingFields[0]->playingField) o->fitness = 0;
    simulation b(0, &simSettings2, &error, theMainWindow);
    if (error) return false;
    for (int i = 0; i < pfSize; i++) b.playingFields[0]->playingField[i]->fitness = i;

    simulationVariables simSettings3;
    simSettings3.expandingPlayingfield = true;
    simulation c(0, &simSettings3, &error, theMainWindow);
    if (error) return false;


    QVector<int> hits0(pfSize, 0);
    QVector<int> hits1(pfSize, 0);
    QVector<int> hits2(pfSize, 0);
    QVector<int> hits3(pfSize, 0);
    QVector<int> hits4(pfSize, 0);
    QVector<int> hits5(pfSize, 0);

    if (theMainWindow)
    {
        theMainWindow->addProgressBar(0, 100000);
        theMainWindow->setStatus("Testing overwrite code.");
    }
    for (int i = 0; i < 100000; i++)
    {
        if (theMainWindow)
            theMainWindow->setProgressBar(i);
        hits0[x.calculateOverwrite(x.playingFields[0], 0)]++;
        hits1[y.calculateOverwrite(y.playingFields[0], 0)]++;
        hits2[z.calculateOverwrite(z.playingFields[0], 0)]++;
        hits3[a.calculateOverwrite(a.playingFields[0], 0)]++;
        hits4[b.calculateOverwrite(b.playingFields[0], 0)]++;
        hits5[c.calculateOverwrite(c.playingFields[0], 4)]++;
    }
    if (theMainWindow)
        theMainWindow->hideProgressBar();

    out << "<table><tr>"
        "<th>Position</th>"
        "<th></th>"
        "<th>Equal fitness</th>"
        "<th></th>"
        "<th>Fittest at top</th>"
        "<th></th>"
        "<th>Half top fitness</th>"
        "<th></th>"
        "<th>Equal Fitness - random overwrite</th>"
        "<th></th>"
        "<th>Fittest at top - random overwrite</th>"
        "<th></th>"
        "<th>Expanding - species #4</th>"
        "</tr>";

    for (int i = 0; i < pfSize; i++)
    {
        out << "<tr><td>" << i << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits0[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits1[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits2[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits3[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits4[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits5[i] << "</td></tr>";
    }
    out << "</table>\nRunning a bunch of tests comparing expected distribution to actual. Any failures printed below.\n";
    for (int i = 0; i < pfSize - 1; i++)
    {
        double hits0d = static_cast<double>(hits0[i]) / static_cast<double>(hits0[i + 1]);
        if (hits0d < 0.8 || hits0d > 1.2)
        {
            out << "Failed at hits0 - divisor is " << hits0d << "\n";
            testFlag = false;
        }

        if (hits1[i] != 0)
        {
            out << "Failed at hits1";
            testFlag = false;
        }

        if (i < (pfSize / 2) - 1)
        {
            double hits2d = static_cast<double>(hits2[i]) / static_cast<double>(hits2[i + 1]);
            if (hits2d < 0.8 || hits2d > 1.2)
            {
                out << "Failed at hits2 - divisor is " << hits2d << "\n";
                testFlag = false;
            }
        }
        else if (i > (pfSize / 2) && hits2[i] != 0)
        {
            out << "Failed at hits2 - should be zero at entry on playing field " << i << ".\n";
            testFlag = false;
        }

        double hits3d = static_cast<double>(hits3[i]) / static_cast<double>(hits3[i + 1]);
        if (hits3d < 0.8 || hits3d > 1.2)
        {
            out << "Failed at hits3 - divisor is " << hits3d << "\n";
            testFlag = false;
        }

        double hits4d = static_cast<double>(hits4[i]) / static_cast<double>(hits4[i + 1]);
        if (hits4d < 0.8 || hits4d > 1.2)
        {
            out << "Failed at hits4 - divisor is " << hits4d << "\n";
            testFlag = false;
        }

        if (i == 4 && hits5[i] < 100000)
        {
            testFlag = false;
            out << "Failed at hits5 - test 1\n";
        }
        else if (i != 4 && hits5[i] != 0)
        {
            testFlag = false;
            out << "Failed at hits5 - test 2\n";
        }
    }

    if (testFlag) out << "\nOverwrite species tests passed.\n";
    return testFlag;
}

//Test apply perturbation
bool testinternal::testNine(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing perturbations. First perturbation start.\n\n";

    for (int i = 0; i < 3; i++)
    {
        simulationVariables simSettings;
        simSettings.environmentalPerturbation = true;
        simSettings.playingfieldNumber = 2;
        simSettings.playingfieldSize = 12; //Set the size to 12 which was the default for v2.0.0
        simSettings.maskNumber = 3; //Set to 3 which was the default for v2.0.0
        simSettings.mixingPerturbation = true;
        simSettings.mixingProbabilityOneToZero = 1;
        simSettings.mixingProbabilityZeroToOne = 2;
        if (i == 0) out << "Playing field masks mode is identical - 0 should match 1.\n";
        if (i > 0)
        {
            simSettings.playingfieldMasksMode = i;
            out << "Playing field masks mode is non-identical by perturbation - 0 should not match 1.\n";
        }

        simulation x(0, &simSettings, &error, theMainWindow);
        if (error)return false;
        x.iterations = 66;
        simulation y(0, &simSettings, &error, theMainWindow);
        if (error) return false;
        y.playingFields[0]->masks = x.playingFields[0]->masks;
        y.playingFields[1]->masks = x.playingFields[1]->masks;

        QString masksX0(x.printMasks(x.playingFields, 0));
        QString masksX1(x.printMasks(x.playingFields, 1));
        QString masksY0(y.printMasks(y.playingFields, 0));
        QString masksY1(y.printMasks(y.playingFields, 1));

        out << "Playingfields pre-perturbation X and Y should be identical (Y is copy of X):\n";
        out << "X0:" << QCryptographicHash::hash(masksX0.toUtf8(), QCryptographicHash::Md5).toHex() << "\n";
        out << "Y0:" << QCryptographicHash::hash(masksY0.toUtf8(), QCryptographicHash::Md5).toHex() << "\n";
        out << "X1:" << QCryptographicHash::hash(masksX1.toUtf8(), QCryptographicHash::Md5).toHex() << "\n";
        out << "Y1:" << QCryptographicHash::hash(masksY1.toUtf8(), QCryptographicHash::Md5).toHex() << "\n";
        QString flagString;
        if (x.playingFields[0]->masks != y.playingFields[0]->masks || x.playingFields[1]->masks != y.playingFields[1]->masks) testFlag = false;
        flagString = testFlag ? "true" : "false";
        out << "TREvoSim has tested whether these are identical and outputs " << flagString << ".\n\n";

        //Apply perturbations should update a bunch of varianles, and bork the masks - check this!
        x.applyPerturbation();

        masksX0 = x.printMasks(x.playingFields, 0);
        masksX1 = x.printMasks(x.playingFields, 1);
        masksY0 = y.printMasks(y.playingFields, 0);
        masksY1 = y.printMasks(y.playingFields, 1);
        out << "Playingfields post-perturbation of X: X and Y masks should no longer be identical:\n";
        out << "X0:" << QCryptographicHash::hash(masksX0.toUtf8(), QCryptographicHash::Md5).toHex() << "\n";
        out << "Y0:" << QCryptographicHash::hash(masksY0.toUtf8(), QCryptographicHash::Md5).toHex() << "\n";
        out << "X1:" << QCryptographicHash::hash(masksX1.toUtf8(), QCryptographicHash::Md5).toHex() << "\n";
        out << "Y1:" << QCryptographicHash::hash(masksY1.toUtf8(), QCryptographicHash::Md5).toHex() << "\n";
        if (x.playingFields[0]->masks == y.playingFields[0]->masks || x.playingFields[1]->masks == y.playingFields[0]->masks) testFlag = false;
        flagString = testFlag ? "true" : "false";
        out << "TREvoSim has tested that these are not identical and outputs " << flagString << ".\n";

        if (i == 0)
        {
            if  (x.playingFields[0]->masks != x.playingFields[1]->masks) testFlag = false;
            flagString = testFlag ? "true" : "false";
            out << "\nIn X and Y should PFs should be identical (i.e. X0==Y0), TREvoSim has tested that they are, and outputs " << flagString << ".\n";

            if (x.perturbationStart != 66) testFlag = false;
            flagString = testFlag ? "true" : "false";
            out << "Perturbation starts should be 66. It is " << x.perturbationStart << ". TREvoSim has tested this and outputs " << flagString << ".\n";
            if (x.perturbationOccurring != 1) testFlag = false;
            flagString = testFlag ? "true" : "false";
            out << "PerturbationOccurring should be 1. It is " << x.perturbationOccurring << ". TREvoSim has tested this and outputs " << flagString << ".\n";
            if (x.perturbationEnd != 72) testFlag = false;
            flagString = testFlag ? "true" : "false";
            out << "Perturbation ends should be 72. It is " << x.perturbationEnd << ". TREvoSim has tested this and outputs " << flagString << ".\n";
            if (x.environmentalPerturbationCopyRate == 10) testFlag = false;
            flagString = testFlag ? "true" : "false";
            out << "EnvironmentalPerturbationCopyRate needs to be >0. It is " << x.environmentalPerturbationCopyRate << ". TREvoSim has tested this and outputs " << flagString << ".\n";

            int cnt = 0;
            for (int l = 0; l < simSettings.playingfieldNumber; l++)
                for (int k = 0; k < simSettings.environmentNumber; k++)
                    for (int j = 0; j < simSettings.maskNumber; j++)
                        for (int i = 0; i < x.runFitnessSize; i++)
                            if (x.environmentalPerturbationOverwriting[l][k][j][i])cnt++;
            if (cnt != 0) testFlag = false;
            out << "EnvironmentalPerturbationOverwriting is " << cnt << " it should be zero here right now.\n";
            x.applyPerturbation();
            cnt = 0;
            for (int l = 0; l < simSettings.playingfieldNumber; l++)
                for (int k = 0; k < simSettings.environmentNumber; k++)
                    for (int j = 0; j < simSettings.maskNumber; j++)
                        for (int i = 0; i < x.runFitnessSize; i++)
                            if (x.environmentalPerturbationOverwriting[l][k][j][i])cnt++;
            if (cnt != 108) testFlag = false;
            out << "EnvironmentalPerturbationOverwriting is " << cnt << " it should be 108 right now.\n";
            x.applyPerturbation();
            cnt = 0;
            for (int l = 0; l < simSettings.playingfieldNumber; l++)
                for (int k = 0; k < simSettings.environmentNumber; k++)
                    for (int j = 0; j < simSettings.maskNumber; j++)
                        for (int i = 0; i < x.runFitnessSize; i++)
                            if (x.environmentalPerturbationOverwriting[l][k][j][i])cnt++;
            if (cnt != 216) testFlag = false;
            out << "EnvironmentalPerturbationOverwriting is " << cnt << " it should be double figure above right now.\n";
            if (x.runMixingProbabilityOneToZero != 10) testFlag = false;
            if (x.runMixingProbabilityZeroToOne != 20) testFlag = false;
            flagString = testFlag ? "true" : "false";
            out << "Mixing probability one to zero should be 10. It is " << x.runMixingProbabilityOneToZero << ".\n";
            out << "Mixing probability one to zero should be 20. It is " << x.runMixingProbabilityZeroToOne << ".\n";
            out << "TREvoSim has tested this and outputs " << flagString << ".\n";
            x.iterations = 72;
            x.applyPerturbation();
            if (x.perturbationOccurring != 2) testFlag = false;
            flagString = testFlag ? "true" : "false";
            out << "Ended perturbation - PerturbationOccurring should be 2. It is " << x.perturbationOccurring << ". TREvoSim has tested this and outputs " << flagString << ".\n";
            if (x.runMixingProbabilityOneToZero != 1) testFlag = false;
            if (x.runMixingProbabilityZeroToOne != 2) testFlag = false;
            flagString = testFlag ? "true" : "false";
            out << "Mixing probability one to zero should be 1. It is " << x.runMixingProbabilityOneToZero << ".\n";
            out << "Mixing probability one to zero should be 2. It is " << x.runMixingProbabilityZeroToOne << ".\n\n";
        }
        else
        {
            if  (x.playingFields[0]->masks == x.playingFields[1]->masks) testFlag = false;
            flagString = testFlag ? "true" : "false";
            out << "In X and Y should PFs should not be identical (i.e. X0!=Y0), TREvoSim has tested that they are not, and outputs " << flagString << ".\n\n";
        }
        out << "--\n\n";
    }

    if (testFlag) out << "\nPerturbation tests passed.\n";
    return testFlag;
}

//Now test strip uninformative
bool testinternal::testTen(QString &outString)
{
    // To do - test coding v.s. non-coding? Needs to be done prior to release, but perhaps this needs a bit of a think of what should be released?
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Testing strip uninformative.\n\n";

    simulationVariables simSettings;
    simSettings.runForTaxa = 20;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;

    QVector <Organism *> speciesList;
    int count = 0;
    for (int i = 0; i < 20; i++)
    {
        Organism *org = new Organism(50, false);
        QList<bool> genome;

        for (int j = 0; j < 50; j++)
        {
            if (j > count) genome.append(false);
            else genome.append(true);
        }
        org->setGenome(genome);
        speciesList.append(org);
        count++;
    }

    out << "Stripping uninformative characters from matrix below with " << speciesList[0]->genome.length() << " characters.\n";
    out << x.printMatrix(speciesList);
    x.runFitnessSize = 50;
    x.runGenomeSize = 50;

    QList <int> uninformativeCoding;
    QList <int> uninformativeNonCoding;

    //Test for informative
    x.testForUninformative(speciesList, uninformativeCoding, uninformativeNonCoding);
    x.stripUninformativeCharacters(speciesList, uninformativeCoding, uninformativeNonCoding);
    out << "\nThere should be 17 informative characters.\n";
    if (speciesList[0]->genome.length() != 17)testFlag = false;
    out << "Stripped of uninformative characters, this matrix has " << speciesList[0]->genome.length() << " characters.\n";
    out << x.printMatrix(speciesList);

    if (testFlag) out << "\nUninformative tests passed.\n";
    return testFlag;
}

//Check for unresolvable taxa
bool testinternal::testEleven(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing check for unresolvable taxa.\n\n";

    simulationVariables simSettings;
    simSettings.runForTaxa = 20;
    simSettings.genomeSize = 50;
    simSettings.fitnessSize = 50;
    simSettings.speciesSelectSize = 50;
    simSettings.test = 11;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;

    QVector <Organism *> speciesList;
    for (int i = 0; i < 20; i += 2)
    {
        Organism *org = new Organism(50, false);
        org->initialise(50);
        speciesList.append(org);
        speciesList.append(org);
    }

    out << "Checking unresolvable count for matrix below - should be 10 groups of two.\n";
    out << x.printMatrix(speciesList);

    QString unresolvableTaxonGroups;
    int unresolvableCount;
    testFlag = !x.checkForUnresolvableTaxa(speciesList, unresolvableTaxonGroups, unresolvableCount);
    out << "Recovered " << unresolvableCount << " unresolvable taxa. These were:\n";
    out << unresolvableTaxonGroups;
    QString flagString = testFlag ? "true" : "false";
    out << "\nUnresolvable cutoff is five, so this should return false. TREvoSim tested that this was returning false, and the test reported " << flagString << ".\n";

    simSettings.unresolvableCutoff = 20;
    unresolvableTaxonGroups.clear();
    simulation y(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    testFlag = y.checkForUnresolvableTaxa(speciesList, unresolvableTaxonGroups, unresolvableCount);
    flagString = testFlag ? "true" : "false";
    out << "\nNow changed unresolvable cutoff to 20, and checked for resolvable again, which should return true. Tested this and returned " << flagString  << ".\n";

    if (testFlag) out << "\nUnresolvable tests passed.\n";
    return testFlag;
}

//Check memory
bool testinternal::testTwelve(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    simulationVariables simSettings;
    simSettings.playingfieldNumber = 2;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;

    QVector <Organism *> speciesList;
    for (int i = 0; i < 20; i ++)
    {
        Organism *org = new Organism(50, false);
        org->initialise(50);
        speciesList.append(org);
    }

    out << "Testing memory release for data structures.\n\n";
    out << "TREvoSim has allocated memory for twenty species, which is taking up " << speciesList.capacity() * sizeof (speciesList) << " and two playing fields, which is using " <<
        x.playingFields.capacity() * sizeof (x.playingFields) << ".\n";
    out << "Clearing memory.\n";
    x.clearVectors(x.playingFields, speciesList);
    out << "TREvoSim is now taking up " << speciesList.capacity() * sizeof (speciesList) << " for the species list, and the two playing fields, are using " <<
        x.playingFields.capacity() * sizeof (x.playingFields) << ".\n";

    if (speciesList.capacity() * sizeof (speciesList) > 0 ||  x.playingFields.capacity() * sizeof (x.playingFields) > 0)testFlag = false;

    if (testFlag) out << "\nMemory tests passed (note that RJG also tests for leaks using valgrind prior to release).\n";
    return testFlag;
}

//Check extinction
bool testinternal::testThirteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing species extinction.\n\n";

    QVector <Organism *> speciesList;
    for (int i = 0; i < 20; i ++)
    {
        Organism *org = new Organism(50, false);
        org->initialise(50);
        speciesList.append(org);
    }

    simulationVariables simSettings;
    simSettings.playingfieldNumber = 2;
    simSettings.genomeSize = 50;
    simSettings.fitnessSize = 50;
    simSettings.speciesSelectSize = 50;

    simulation x(0, &simSettings, &error, theMainWindow);
    for (int i = 0; i < 50; i++)x.extinctList.append(false);

    if (error) return false;

    for (auto pf : qAsConst(x.playingFields))
        for (auto o : qAsConst(pf->playingField))
            o->initialise(simSettings.genomeSize);

    x.iterations = 66;

    if (theMainWindow)
        theMainWindow->resizeGrid(simSettings.runForTaxa, simSettings.genomeSize);

    QList <bool> genome;
    for (int i = 0; i < 50; i++) genome.append(false);
    x.playingFields[0]->playingField[10]->speciesID = 1;
    x.playingFields[0]->playingField[10]->setGenome(genome);
    x.playingFields[1]->playingField[6]->speciesID = 1;
    x.playingFields[1]->playingField[6]->setGenome(genome);


    QHash<QString, QVector <int> > extinct = x.checkForExtinct(speciesList);

    for (auto s : qAsConst(extinct)) x.speciesExtinction(speciesList[s[0]], x.playingFields[s[1]]->playingField[s[2]], (x.iterations + 1), simSettings.sansomianSpeciation, simSettings.stochasticLayer,
                                                             true);

    for (int i = 0; i < 20; i ++)
        if (speciesList[i]->extinct != 0)
        {
            testFlag = false;
            out << "Set species extinction to 66 for species 1 when there are two instances - fail.\n\n";
            return testFlag;
        }
    if (testFlag) out << "Nothing extinct in first part of test - passed.\nNext test should have two extinct taxa.\n";

    x.playingFields[1]->playingField[6]->speciesID = 2;

    extinct = x.checkForExtinct(speciesList);

    for (auto s : qAsConst(extinct)) out << "Species is extinct: " << s[0] << "\n";
    for (auto s : qAsConst(extinct)) x.speciesExtinction(speciesList[s[0]], x.playingFields[s[1]]->playingField[s[2]], (x.iterations + 1), simSettings.sansomianSpeciation, simSettings.stochasticLayer,
                                                             true);

    for (int i = 0; i < 20; i ++)
        if (speciesList[i]->extinct != 0)
        {
            out << "Extinct at " << speciesList[i]->extinct << "\nGenome should be all zeros. It is: ";
            for (auto b : qAsConst(speciesList[i]->genome))
            {
                out << (b ? "1" : "0");
                if (b)
                {
                    testFlag = false;
                    out << "Genome is incorrect in test 13.\n";
                }
            }
            if (speciesList[i]->extinct != 67)
            {
                testFlag = false;
                out << "Set species extinction to 67 for species 1 when there are two instances - fail.\n\n";
            }
            out << "\n";
        }

    out << "Now check non-Sansomian\n";
    x.iterations = 68;
    x.playingFields[0]->playingField[11]->speciesID = 4;
    x.playingFields[1]->playingField[6]->speciesID = 1;
    extinct = x.checkForExtinct(speciesList);
    for (auto s : qAsConst(extinct)) out << "Species is extinct: " << s[0] << "\n";
    for (auto s : qAsConst(extinct)) x.speciesExtinction(speciesList[s[0]], x.playingFields[s[1]]->playingField[s[2]], (x.iterations + 1), simSettings.sansomianSpeciation, simSettings.stochasticLayer,
                                                             true);

    for (int i = 0; i < 20; i ++)
        if (speciesList[i]->extinct == 69)
        {
            out << "Extinct at " << speciesList[i]->extinct << "\nGenome should not be all zeros. It is: ";
            int count = 0;
            for (auto b : qAsConst(speciesList[i]->genome))
            {
                out << (b ? "1" : "0");
                if (!b) count++;
            }
            if (count == 50)
            {
                testFlag = false;
                out << "Sansomian in species extinction not working.\n\n";
            }
            out << "\n";
        }


    if (testFlag) out << "\nExtinction tests passed.\n";

    simulationVariables simSettingsReset;
    if (theMainWindow)
    {
        theMainWindow->resizeGrid(simSettingsReset.runForTaxa, simSettingsReset.genomeSize);
        theMainWindow->resetDisplays();
    }

    return testFlag;
}

//Check count difference to parent
bool testinternal::testFourteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing difference to parent.\n\n";

    //Initialised to false
    Organism org(50, false);
    for (auto &p : org.parentGenome) p = true;

    simulationVariables simSettings;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;

    int diff = x.differenceToParent(&org, 50);

    out << "Set genome to false, parent to true, select size 50. Should be 50, returns " << diff << ".\n";
    if (diff != 50)
    {
        testFlag = false;
    }

    diff = x.differenceToParent(&org, 25);

    out << "Set genome to false, parent to true, select size 25. Should be 25, returns " << diff << ".\n";
    if (diff != 25)
    {
        testFlag = false;
    }

    for (auto &p : org.parentGenome) p = false;
    diff = x.differenceToParent(&org, 50);

    out << "Set genome to false, parent to false, select size 50. Should be 0, returns " << diff << ".\n";
    if (diff != 0)
    {
        testFlag = false;
    }

    diff = x.differenceToParent(&org, 25);

    out << "Set genome to false, parent to false, select size 25. Should be 0, returns " << diff << ".\n";
    if (diff != 0)
    {
        testFlag = false;
    }

    return testFlag;
}

//Check print matrix function
bool testinternal::testFifteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Check print matrix.\n\n";

    QVector <Organism *> speciesList;
    for (int i = 0; i < 20; i ++)
    {
        Organism *org = new Organism(50, false);
        speciesList.append(org);
    }
    simulationVariables simSettings;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;

    out << "Should be all zeros.\n" << x.printMatrix(speciesList);

    return testFlag;
}

//Check print tree
bool testinternal::testSixteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Check print tree.\n\n";

    QVector <Organism *> speciesList;
    for (int i = 0; i < 8; i ++)
    {
        Organism *org = new Organism(50, false);
        org->speciesID = i;
        speciesList.append(org);
    }

    //Plotted tree in ape
    /*
     library(ape);

    tips<-vector();
    for(i in 1:8)
    {
    tips[i]<-paste("Species_",i-1,sep="");
    }

    tree <- rtree(8,TRUE,br=sample(1:10, replace=T), tip.label = tips);
    plot(tree);
    edgelabels(tree$edge.length, bg="black", col="white", font=2);
    write.tree(tree);
    */
    //Resulting tree (note below I've changed numbering to reflect the phangorn = true tag: ((Species_0:8,((Species_5:10,Species_3:7):9,(((Species_7:7,Species_4:10):6,Species_6:8):7,Species_2:6):2):6):10,Species_1:9);
    speciesList[0]->children = {1, 2};
    speciesList[2]->children = {3, 4};
    speciesList[3]->children = {5};
    speciesList[4]->children = {6, 7};

    speciesList[0]->born = 0;
    speciesList[0]->cladogenesis = 10;
    speciesList[0]->extinct = 18;

    speciesList[1]->born = 0;
    speciesList[1]->cladogenesis = 0;
    speciesList[1]->extinct = 9;

    speciesList[2]->born = 10;
    speciesList[2]->cladogenesis = 18;
    speciesList[2]->extinct = 24;

    speciesList[3]->born = 16;
    speciesList[3]->cladogenesis = 25;
    speciesList[3]->extinct = 32;

    speciesList[4]->born = 18;
    speciesList[4]->cladogenesis = 31;
    speciesList[4]->extinct = 41;

    speciesList[5]->born = 25;
    speciesList[5]->cladogenesis = 25;
    speciesList[5]->extinct = 35;

    speciesList[6]->born = 25;
    speciesList[6]->cladogenesis = 25;
    speciesList[6]->extinct = 33;

    speciesList[7]->born = 31;
    speciesList[7]->cladogenesis = 31;
    speciesList[7]->extinct = 38;

    simulationVariables simSettings;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    QString tree(x.printNewickWithBranchLengths(0, speciesList, true));
    tree.replace("S_0", "Species_");
    out << "Tree from R:\n((Species_1:8,((Species_6:10,Species_4:7):9,(((Species_8:7,Species_5:10):6,Species_7:8):7,Species_3:6):2):6):10,Species_2:9);\nPrinted using function:\n" <<
        tree << ";\n";

    out << "\nTo check these are idential, paste the following into R (tree2 is printed using function):\n\n";

    out << "library(ape);\n";
    out << "tree&lt;-read.tree(text = \"((Species_1:8,((Species_6:10,Species_4:7):9,(((Species_8:7,Species_5:10):6,Species_7:8):7,Species_3:6):2):6):10,Species_2:9);\")\n";
    out << "tree2&lt;-read.tree(text = \"" << tree << ";\");\n";
    out << "plot(tree);\n";
    out << "edgelabels(tree$edge.length, bg = \"black\", col = \"white\", font = 2);\n";
    out << "plot(tree2);\n";
    out << "edgelabels(tree2$edge.length, bg = \"black\", col = \"white\", font = 2);\n";
    out << "comparePhylo(tree, tree2, plot = TRUE, use.edge.length = TRUE);\n";
    out << "all.equal(tree, tree2);\n";

    out << "This is not tested within TREvoSim, but (bearing in mind that some of the clades are rotated) you can also tell if the two Newick strings are identical or not from above text.";

    return testFlag;
}

//Check ecosystem engineering
bool testinternal::testSeventeen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Check ecosystem engineers.\n\n";

    simulationVariables simSettings;
    simSettings.runForIterations = 200;
    simSettings.runMode = RUN_MODE_ITERATION;
    simSettings.test = 17;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    x.run();

    simSettings.ecosystemEngineers = true;

    QVector <Organism *> speciesList;
    for (int i = 0; i < x.speciesCount + 1; i ++)
    {
        Organism *org = new Organism(50, false);
        speciesList.append(org);
    }

    //Apply only once, but on second time - should have no effect
    out << "Apply EE with persistent off, for second time - should have no effect\n";
    x.ecosystemEngineeringOccurring = 2;
    simSettings.ecosystemEngineersArePersistent = false;
    x.applyEcosystemEngineering(speciesList, false);

    int total = 0;
    for (auto p : qAsConst(x.playingFields))
        for (auto o : p->playingField)
            if (o->ecosystemEngineer) total ++;

    out << "Have applied EE: count of organisms with EE status in playing field should be 0. It is : " << total << ".\n";
    if (total != 0) testFlag = false;

    bool identical = false;

    for (auto p : qAsConst(x.playingFields))
        for (auto o : p->playingField)
            if (o->ecosystemEngineer)
                for (auto &m : p->masks)
                    for (auto &n : m)
                    {
                        if (o->genome == n) identical = true;
                    }
    out << "Have applied EE and checked that no mask is identical to genome. Check returned: " << identical << ".\n";
    if (identical == true) testFlag = false;

    total = 0;
    for (auto o : speciesList)
        if (o->ecosystemEngineer)
            total ++;
    out << "Have applied EE: count of organisms with EE status in species list should be 0. It is : " << total << ".\n\n";
    if (total != 0) testFlag = false;


    //Apply once, first time
    out << "Apply EE once, for first time - should have an effect.\n";
    x.ecosystemEngineeringOccurring = 1;
    x.applyEcosystemEngineering(speciesList, false);

    total = 0;
    for (auto p : qAsConst(x.playingFields))
        for (auto o : p->playingField)
            if (o->ecosystemEngineer) total ++;

    out << "Have applied EE: count of organisms with EE status in playing field should be >0. It is : " << total << ".\n";
    if (total == 0) testFlag = false;

    identical = false;

    for (auto p : qAsConst(x.playingFields))
        for (auto o : p->playingField)
            if (o->ecosystemEngineer)
                for (auto &m : p->masks)
                    for (auto &n : m)
                    {
                        if (o->genome == n) identical = true;
                    }
    out << "Have applied EE and checked that one mask is identical to genome. Check returned: " << identical << ".\n";
    if (identical == false) testFlag = false;

    total = 0;
    for (auto o : speciesList)
        if (o->ecosystemEngineer)
            total ++;
    out << "Have applied EE: count of organisms with EE status in species list should be 1. It is : " << total << ".\n\n";
    if (total != 1) testFlag = false;


    //Apply again, second time, persistent
    out << "Apply EE for second time - should overwrite mask.\n";
    x.ecosystemEngineeringOccurring = 2;
    simSettings.ecosystemEngineersArePersistent = true;

    for (auto p : qAsConst(x.playingFields))
        for (auto o : p->playingField)
            if (o->ecosystemEngineer)
                for (auto &g : o->genome)
                    g = false;

    x.applyEcosystemEngineering(speciesList, false);

    identical = false;
    for (auto p : qAsConst(x.playingFields))
        for (auto o : p->playingField)
            if (o->ecosystemEngineer)
                for (auto &m : p->masks)
                    for (auto &n : m)
                    {
                        if (o->genome == n) identical = true;
                    }
    out << "Have applied EE and checked that one mask is identical to genome. Check returned: " << identical << ".\n";
    if (identical == false) testFlag = false;

    if (testFlag) out << "\nEE tests passed.\n";

    return testFlag;
}

//Check playing field mixing
bool testinternal::testEighteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Check playing field mixing. The mixing mechanism relies on random numbers to provide a probability of mixing, and as such, on occasions, this test will fail due to the stochastic nature of the process. If this happens, you may want to repeat the test again and see if the warnings dissappear.\n\n";

    simulationVariables simSettings;
    simSettings.runForIterations = 2;
    simSettings.runMode = RUN_MODE_ITERATION;
    simSettings.playingfieldSize = 100;
    simSettings.test = 18;
    simSettings.playingfieldNumber = 2;
    simSettings.mixingProbabilityOneToZero = 20;
    simSettings.mixing = true;
    simulation x(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    x.run();

    QVector <Organism *> speciesList;
    for (int i = 0; i < x.speciesCount + 1; i ++)
    {
        Organism *org = new Organism(50, false);
        org->speciesID = 0;//Set this to zero to stop extinction function complaining
        speciesList.append(org);
    }

    //Assign known genomes to members of each playing field
    bool base = false;
    for (auto p : qAsConst(x.playingFields))
    {
        base = !base;
        for (auto o : p->playingField)
            for (auto &g : o->genome)
                g = base;
    }

    for (int i = 0; i < 100; i++) x.applyPlayingfieldMixing(speciesList);

    int cnt = 0;

    for (auto o : x.playingFields[0]->playingField)
        if (o->genome[0])
            cnt++;

    out << "Playing field zero was originally 100 genomes, all zero. mixingProbabilityOneToZero was set to 20 and mixing applied 100 times, so ~20 should have been overwritten, and the count of all zero genomes should be ~80. It is "
        << cnt << "\n";

    if (cnt < 75 || cnt > 85)
    {
        out << "\n\nThis number seems off what we should expect, although since we're dealing with random numbers, there may be nothing untoward - try repating test\n\n";
        testFlag = false;
    }

    cnt = 0;

    for (auto o : x.playingFields[1]->playingField)
        if (o->genome[0])
            cnt++;

    out << "Playing field one was originally 100 genomes, all ones. This should still be the same as mixingProbabilityZeroToOne is zero, and so we should count no all zero genomes. Count is " << cnt <<
        "\n\n";

    if (cnt != 0)
    {
        testFlag = false;
    }

    simSettings.mixingProbabilityOneToZero = 0;
    simSettings.mixingProbabilityZeroToOne = 20;

    simulation y(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    y.run();

    //Assign known genomes to members of each playing field
    base = false;
    for (auto p : qAsConst(y.playingFields))
    {
        base = !base;
        for (auto o : p->playingField)
            for (auto &g : o->genome)
                g = base;
    }

    for (int i = 0; i < 100; i++) y.applyPlayingfieldMixing(speciesList);

    cnt = 0;

    for (auto o : y.playingFields[0]->playingField)
        if (o->genome[0])
            cnt++;

    out << "Playing field zero was originally 100 genomes, all zero. This should still be the same as mixingProbabilityOneToZero is zero, so none should have been overwritten, and the count of all zero genomes should be 100. It is "
        << cnt << "\n";

    if (cnt != 100)
    {
        testFlag = false;
    }

    cnt = 0;

    for (auto o : y.playingFields[1]->playingField)
        if (o->genome[0])
            cnt++;

    out << "Playing field one was originally 100 genomes, all ones. mixingProbabilityZeroToOne was set to 20 and and mixing applied 100 times, and so we should count ~20 all zero genomes. Count is " <<
        cnt << "\n\n";

    if (cnt > 25 || cnt < 15)
    {
        out << "\n\nThis number seems off what we should expect, although since we're dealing with random numbers, there may be nothing untoward - try repating test\n\n";
        testFlag = false;
    }


    simSettings.mixingProbabilityOneToZero = 50;
    simSettings.mixingProbabilityZeroToOne = 0;
    simSettings.playingfieldNumber = 3;

    simulation z(0, &simSettings, &error, theMainWindow);
    if (error) return false;
    z.run();

    //Assign known genomes to members of each playing field
    base = false;
    for (auto p : qAsConst(z.playingFields))
    {
        base = !base;
        for (auto o : p->playingField)
            for (auto &g : o->genome)
                g = base;
    }

    for (int i = 0; i < 100; i++) z.applyPlayingfieldMixing(speciesList);

    cnt = 0;

    for (auto o : z.playingFields[0]->playingField)
        if (o->genome[0])
            cnt++;

    out << "Now testing three playing fields. Playing field mixing was set to fifty, then repeated 100 times, and PF 0 and 3 were all zeros, PF1 was all ones. As such, we should have ~25 mixed individuals in PF0 & PF3 (though slightly fewer in PF0 as they have been overwritten by those from PF3). Playing field zero count of organisms that are all zero should be ~75-85. It is "
        << cnt << "\n";

    if (cnt < 65 || cnt > 90)
    {
        out << "\n\nThis number seems off what we should expect, although since we're dealing with random numbers, there may be nothing untoward - try repating test\n\n";
        testFlag = false;
    }

    cnt = 0;

    for (auto o : z.playingFields[1]->playingField)
        if (o->genome[0])
            cnt++;

    out << "PF1 was originally 100 genomes, all ones. Some of these will have been overwritten from PF0 and PF3 - around 35 all zeros would be sensible. Count is " << cnt << "\n\n";

    if (cnt > 45 || cnt < 25)
    {
        out << "\n\nThis number seems off what we should expect, although since we're dealing with random numbers, there may be nothing untoward - try repating test\n\n";
        testFlag = false;
    }

    cnt = 0;
    for (auto o : z.playingFields[2]->playingField)
        if (o->genome[0])
            cnt++;

    out << "PF3 is similar to PF1, and and thus there should be ~25 all ones. Count of all zeros is " << cnt << "\n\n";

    if (cnt > 85 || cnt < 65)
    {
        out << "\n\nThis number seems off what we should expect, although since we're dealing with random numbers, there may be nothing untoward - try repating test\n\n";
        testFlag = false;
    }

    return testFlag;
}

bool testinternal::testNineteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool testinternal::testTwenty(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}
