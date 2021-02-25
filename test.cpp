#include "test.h"
#include <QCryptographicHash>

test::test(MainWindow *theMainWindowCon)
{
    theMainWindow = theMainWindowCon;
}

bool test::callTest(int i, QString &outString)
{
    bool pass;
    switch (i)
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
bool test::testZero(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Testing fitness algorithm.\n\n";

    //Create default setting sonject and then a simulation object for the test
    simulationVariables simSettings;
    simSettings.genomeSize = 50;
    //RJG - some stuff redacted for 2.0.0 - email if you're interested
    simulation x(theMainWindow, 0, &simSettings);

    //Fitness requires an organism - create an organism with 50 bits, no stochastic genome, all bits are initialised to zero
    Organism org(simSettings.genomeSize);
    out << "Organism genome is: " << x.printGenomeString(&org) << "\n";

    //Now set masks in simulation to 1
    for (auto p : x.playingFields)
        for (int k = 0; k < simSettings.environmentNumber; k++)
            for (int j = 0; j < simSettings.maskNumber; j++)
                for (auto &i : p->masks[k][j]) i = true;

    QString maskString = x.printMasks(x.playingFields);
    QStringList l = maskString .split('\n');
    out << "Masks are:\n" << l[2]  << "\n" << l[3] << "\n" << l[4] << "\n";

    int fitness = x.fitness(&org, x.playingFields[0]->masks, simSettings.genomeSize, simSettings.fitnessTarget);
    if (fitness != 150) testFlag = false;
    out <<  "Fitness, with fitness target of 0, is " << fitness << ". It should be 150.\n";

    simSettings.fitnessTarget = 75;
    fitness = x.fitness(&org, x.playingFields[0]->masks, simSettings.genomeSize, simSettings.fitnessTarget);
    if (fitness != 75) testFlag = false;
    out <<  "Fitness, with fitness target of 75, is " << fitness << ". It should be 75.\n";

    for (int i = 0; i < 25; i++)org.genome[i] = 1;
    out << "Fitness target is still 75, genome is now: " << x.printGenomeString(&org) << "\n";
    fitness = x.fitness(&org, x.playingFields[0]->masks, simSettings.genomeSize, simSettings.fitnessTarget);
    if (fitness != 0) testFlag = false;
    out <<  "Fitness is " << fitness << ". It should be 0.\n";

    simSettings.maskNumber = 2;
    simSettings.fitnessTarget = 50;
    out << "Set masks to two, and fitness target back to 50.\n";
    simulation y(theMainWindow, 0, &simSettings);
    //Now set masks in simulation to 1
    for (auto p : y.playingFields)
        for (int k = 0; k < simSettings.environmentNumber; k++)
            for (int j = 0; j < simSettings.maskNumber; j++)
                for (auto &i : p->masks[k][j]) i = true;

    for (int i = 0; i < simSettings.genomeSize; i++)org.genome[i] = 1;
    maskString = y.printMasks(y.playingFields);
    l = maskString .split('\n');
    out << "Organism genome is: " << y.printGenomeString(&org) << "\n";
    out << "Masks are:\n" << l[2]  << "\n" << l[3] << "\n";
    fitness = y.fitness(&org, y.playingFields[0]->masks, simSettings.genomeSize, simSettings.fitnessTarget);
    if (fitness != 50) testFlag = false;
    out <<  "Fitness is " << fitness << ". It should be 50.\n";

    if (testFlag) out << "\nFitness tests passed.\n";

    return testFlag;
}

//Test mask initialisation, and behaviour with multiple playing fields in different mask modes
bool test::testOne(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Mask mode at initialisation. If there is more than one playing field this will set the  playing field masks/environments to be the same or different as the mode dictates.\n";

    //Create default setting sonject and then a simulation object for the test
    simulationVariables simSettings;
    simSettings.genomeSize = 50;
    //RJG - some stuff redacted for 2.0.0 - email if you're interested

    //Mode identical
    simSettings.playingfieldMasksMode = 0;
    simSettings.playingfieldNumber = 3;
    simulation x(theMainWindow, 0, &simSettings);
    if (x.playingFields[0]->masks != x.playingFields[1]->masks || x.playingFields[1]->masks != x.playingFields[2]->masks) testFlag = false;
    QString flagString = testFlag ? "true" : "false";

    QString masks(x.printMasks(x.playingFields, 1));
    QString masks2(x.printMasks(x.playingFields, 2));

    out << "\nMode identical:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are identical (they should be) and outputs " << flagString << ".\n";

    //Mode independent
    simSettings.playingfieldMasksMode = 1;
    simulation y(theMainWindow, 0, &simSettings);
    if (y.playingFields[0]->masks == y.playingFields[1]->masks || y.playingFields[1]->masks == y.playingFields[2]->masks) testFlag = false;
    flagString = testFlag ? "true" : "false";

    masks = (y.printMasks(y.playingFields, 1));
    masks2 = (y.printMasks(y.playingFields, 2));

    out << "\nMode independent:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are not identical and outputs " << flagString  << ".\n";

    //Mode identical at start
    simSettings.playingfieldMasksMode = 2;
    simulation z(theMainWindow, 0, &simSettings);
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
bool test::testTwo(QString &outString)
{

    bool testFlag = true;
    QTextStream out(&outString);
    out << "Masks after 100 iterations. This will differ based on mode.\n\n";

    //Create default setting object and then a simulation object for the test
    simulationVariables simSettings;
    simSettings.genomeSize = 50;
    //RJG - some stuff redacted for 2.0.0 - email if you're interested
    simSettings.playingfieldNumber = 3;
    simSettings.taxonNumber = 5;
    simSettings.test = 2;

    theMainWindow->resizeGrid(simSettings);

    //Mode identical
    simSettings.playingfieldMasksMode = 0;
    simulation x(theMainWindow, 0, &simSettings);
    x.run();
    if (x.playingFields[0]->masks != x.playingFields[1]->masks || x.playingFields[1]->masks != x.playingFields[2]->masks) testFlag = false;
    QString flagString = testFlag ? "true" : "false";

    QString masks(x.printMasks(x.playingFields, 1));
    QString masks2(x.printMasks(x.playingFields, 2));

    out << "Mode identical:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are identical and outputs " << flagString << ".\n";

    //Mode independent
    simSettings.playingfieldMasksMode = 1;
    simulation y(theMainWindow, 0, &simSettings);
    y.run();
    if (y.playingFields[0]->masks == y.playingFields[1]->masks || y.playingFields[1]->masks == y.playingFields[2]->masks) testFlag = false;
    flagString = testFlag ? "true" : "false";

    masks = (y.printMasks(y.playingFields, 1));
    masks2 = (y.printMasks(y.playingFields, 2));

    out << "\nMode independent:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are not identical and outputs " << flagString  << ".\n";


    //Mode identical at start
    simSettings.playingfieldMasksMode = 2;
    simulation z(theMainWindow, 0, &simSettings);
    z.run();
    if (z.playingFields[0]->masks == z.playingFields[1]->masks || z.playingFields[1]->masks == z.playingFields[2]->masks) testFlag = false;
    flagString = testFlag ? "true" : "false";

    masks = (z.printMasks(z.playingFields, 1));
    masks2 = (z.printMasks(z.playingFields, 2));

    out << "\nMode start identical:\n" << QCryptographicHash::hash(masks.toUtf8(), QCryptographicHash::Md5).toHex() << "\n" << QCryptographicHash::hash(masks2.toUtf8(),
            QCryptographicHash::Md5).toHex() << "\nTREvoSim has tested whether these are not identical and outputs " << flagString << ".\n";

    if (testFlag) out << "\nMask mode tests passed.\n";

    simulationVariables simSettingsReset;
    theMainWindow->resizeGrid(simSettingsReset);
    return testFlag;
}

//Test initialisation of playing fields - should be populated with identical individual
bool test::testThree(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Checking that all organisms are identical after initialisation in all mask modes.\n\n";

    simulationVariables simSettings;
    simSettings.genomeSize = 50;
    //RJG - some stuff redacted for 2.0.0 - email if you're interested
    simSettings.playingfieldNumber = 3;
    simSettings.taxonNumber = 5;
    simSettings.test = 3;
    simSettings.workingLog = true;

    theMainWindow->resizeGrid(simSettings);

    //Mode identical
    simSettings.playingfieldMasksMode = 0;
    simulation x(theMainWindow, 0, &simSettings);
    x.run();

    Organism org(*x.playingFields[0]->playingField[0]);
    for (auto p : x.playingFields)
        for (auto o : p->playingField)
            if (!(*o == org)) testFlag = false;

    QString flagString = testFlag ? "true" : "false";
    out << "Mode identical - checked if all organisms are the same after initialisation and returned " << flagString << ".\n";

    //Mode independent
    simSettings.playingfieldMasksMode = 1;
    simulation y(theMainWindow, 0, &simSettings);
    y.run();

    org = *y.playingFields[0]->playingField[0];
    for (auto p : y.playingFields)
        for (auto o : p->playingField)
            if (!(*o == org)) testFlag = false;
    flagString = testFlag ? "true" : "false";
    out << "Mode independent - checked if all organisms are the same after initialisation and returned " << flagString << ".\n";

    //Mode identical at start
    simSettings.playingfieldMasksMode = 2;
    simulation z(theMainWindow, 0, &simSettings);
    z.run();
    org = *z.playingFields[0]->playingField[0];
    for (auto p : z.playingFields)
        for (auto o : p->playingField)
            if (!(*o == org)) testFlag = false;
    flagString = testFlag ? "true" : "false";
    out << "Mode identical at start - checked if all organisms are the same after initialisation and returned " << flagString << ".\n";

    if (testFlag) out << "\nInitialisation tests passed.\n";

    simulationVariables simSettingsReset;
    theMainWindow->resizeGrid(simSettingsReset);
    return testFlag;
}

//Test stochastic mapping - creation of genome from stochastic layer using user defined map
bool test::testFour(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "\n\nThis tests an experimental feature not present in TREvoSim v2.0.0.\n";
    return testFlag;
}

//Test mutation rates for environment and organism
bool test::testFive(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Testing mutation rates.\n\n";

    theMainWindow->addProgressBar(0, 10000);
    theMainWindow->setStatus("Doing organism mutation tests");

    simulationVariables simSettings;
    simulation x(theMainWindow, 0, &simSettings);

    Organism org1(simSettings.genomeSize);
    int cnt = 0;

    for (int j = 0; j < 10000; j++)
    {
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
    simSettings.playingfieldMasksMode = 2;
    simulation y(theMainWindow, 0, &simSettings);

    QVector <QVector <QVector <bool> > > masks;
    QVector <QVector <QVector <bool> > > masks2;
    int cnts[12] = {0};

    theMainWindow->setStatus("Doing environment mutation tests");

    for (int i = 0; i < 10000; i++)
    {
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
bool test::testSix(QString &outString)
{

    theMainWindow->addProgressBar(0, 100000);
    theMainWindow->setStatus("Testing coin toss.\n");

    bool testFlag = true;
    QTextStream out(&outString);
    out << "Testing coin toss.\n\n";

    simulationVariables simSettings;
    simSettings.playingfieldNumber = 4;
    simulation x(theMainWindow, 0, &simSettings);

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
    theMainWindow->hideProgressBar();

    return testFlag;
}

//Test the new species function - called when SD exceeded
bool test::testSeven(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing new species - new species created at iteration 66 with a genome of all 1's.\n\n";

    simulationVariables simSettings;
    simSettings.genomeSize = 50;
    simulation x(theMainWindow, 0, &simSettings);
    //This should update the counters on new species
    x.iterations = 66;
    x.speciesCount = 14;

    //This will have initialised values - i.e. zero for everything except the genome, which will be 50% 1's
    Organism newSpecies(50);
    newSpecies.initialise(50);
    newSpecies.speciesID = 10;
    for (auto &i : newSpecies.genome) i = true;

    Organism parentSpecies(50);
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
    for (auto i : newSpecies.parentGenome) if (i != 1)testFlag = false;
    out << "New species parent genome: ";
    for (auto i : newSpecies.parentGenome) i ? out << "1" : out << "0";
    out << " (should be all 1s).\n";
    for (auto i : newSpecies.genome) if (i != 1)testFlag = false;
    out << "New species genome: ";
    for (auto i : newSpecies.genome) i ? out << "1" : out << "0";
    out << " (should be all 1s).\n";
    for (auto i : x.playingFields[0]->playingField[8]->parentGenome) if (i != 1)testFlag = false;
    out << "Species 10 parent genome in playing field is now: ";
    for (auto i :  x.playingFields[0]->playingField[8]->parentGenome) i ? out << "1" : out << "0";
    out << " (should be all 1s).\n";

    if (testFlag) out << "\nNew species tests passed.\n";

    return testFlag;
}

//Test which organism to overwrite in playing field
bool test::testEight(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing overwrite code.\n\n";

    simulationVariables simSettings;

    simSettings.randomOverwrite = false;
    simulation x(theMainWindow, 0, &simSettings);
    for (auto &o : x.playingFields[0]->playingField) o->fitness = 0;
    int pfSize = x.playingFields[0]->playingField.length();
    simulation y(theMainWindow, 0, &simSettings);
    for (int i = 0; i < pfSize ; i++) y.playingFields[0]->playingField[i]->fitness = i;
    simulation z(theMainWindow, 0, &simSettings);
    for (int i = 0; i < pfSize ; i++)
        if (i > pfSize / 2) z.playingFields[0]->playingField[i]->fitness = 0;
        else z.playingFields[0]->playingField[i]->fitness = 1;

    simulationVariables simSettings2;
    simSettings2.randomOverwrite = true;
    simulation a(theMainWindow, 0, &simSettings2);
    for (auto &o : a.playingFields[0]->playingField) o->fitness = 0;
    simulation b(theMainWindow, 0, &simSettings2);
    for (int i = 0; i < pfSize; i++) b.playingFields[0]->playingField[i]->fitness = i;

    //int simulation::calculateOverwrite(const playingFieldStructure * pf)
    QVector<int> hits0(pfSize, 0);
    QVector<int> hits1(pfSize, 0);
    QVector<int> hits2(pfSize, 0);
    QVector<int> hits3(pfSize, 0);
    QVector<int> hits4(pfSize, 0);

    theMainWindow->addProgressBar(0, 100000);
    theMainWindow->setStatus("Testing overwrite code.");
    for (int i = 0; i < 100000; i++)
    {
        theMainWindow->setProgressBar(i);
        hits0[x.calculateOverwrite(x.playingFields[0])]++;
        hits1[y.calculateOverwrite(y.playingFields[0])]++;
        hits2[z.calculateOverwrite(z.playingFields[0])]++;
        hits3[a.calculateOverwrite(a.playingFields[0])]++;
        hits4[b.calculateOverwrite(b.playingFields[0])]++;
    }
    theMainWindow->hideProgressBar();

    out << "<table><tr><th>Position</th><th>"
        "</th><th>Equal fitness</th><th>"
        "</th><th>Fittest at top</th><th>"
        "</th><th>Half top fitness</th><th>"
        "</th><th>Equal Fitness - random overwrite</th><th>"
        "</th><th>Fittest at top - random overwrite</th>";

    for (int i = 0; i < pfSize; i++)
    {
        out << "<tr><td>" << i << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits0[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits1[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits2[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits3[i];
        out << "</td><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>" << hits4[i] << "</tr>";
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
    }

    if (testFlag) out << "\nOverwrite species tests passed.\n";
    return testFlag;
}

//Test apply perturbation
bool test::testNine(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "\n\nThis tests an experimental feature not present in TREvoSim v2.0.0.\n";

    return testFlag;
}

//Now test strip uninformative
bool test::testTen(QString &outString)
{
    // To do - test coding v.s. non-coding? Needs to be done prior to release, but perhaps this needs a bit of a think of what should be released?
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Testing strip uninformative.\n\n";

    simulationVariables simSettings;
    simSettings.taxonNumber = 20;
    simulation x(theMainWindow, 0, &simSettings);

    QVector <Organism *> speciesList;
    int count = 0;
    for (int i = 0; i < 20; i++)
    {
        Organism *org = new Organism(50);
        QVector<bool> genome;

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
    x.runGenomeSize = 50;
    x.runGenomeSize = 50;

    QList <int> uninformativeCoding;
    QList <int> uninformativeNonCoding;

    //Test for informative
    x.testForUninformative(speciesList, uninformativeCoding);
    x.stripUninformativeCharacters(speciesList, uninformativeCoding);
    out << "\nThere should be 17 informative characters.\n";
    if (speciesList[0]->genome.length() != 17)testFlag = false;
    out << "Stripped of uninformative characters, this matrix has " << speciesList[0]->genome.length() << " characters.\n";
    out << x.printMatrix(speciesList);

    if (testFlag) out << "\nUninformative tests passed.\n";
    return testFlag;
}

//Check for unresolvable taxa
bool test::testEleven(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing check for unresolvable taxa.\n\n";

    simulationVariables simSettings;
    simSettings.taxonNumber = 20;
    simSettings.genomeSize = 50;
    simSettings.test = 11;
    simulation x(theMainWindow, 0, &simSettings);

    QVector <Organism *> speciesList;
    for (int i = 0; i < 20; i += 2)
    {
        Organism *org = new Organism(50);
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
    simulation y(theMainWindow, 0, &simSettings);
    testFlag = y.checkForUnresolvableTaxa(speciesList, unresolvableTaxonGroups, unresolvableCount);
    flagString = testFlag ? "true" : "false";
    out << "\nNow changed unresolvable cutoff to 20, and checked for resolvable again, which should return true. Tested this and returned " << flagString  << ".\n";

    if (testFlag) out << "\nUnresolvable tests passed.\n";
    return testFlag;
}

//Check memory
bool test::testTwelve(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    simulationVariables simSettings;
    simSettings.playingfieldNumber = 2;
    simulation x(theMainWindow, 0, &simSettings);

    QVector <Organism *> speciesList;
    for (int i = 0; i < 20; i ++)
    {
        Organism *org = new Organism(50);
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
bool test::testThirteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing species extinction.\n\n";

    QVector <Organism *> speciesList;
    for (int i = 0; i < 20; i ++)
    {
        Organism *org = new Organism(50);
        org->initialise(50);
        speciesList.append(org);
    }

    simulationVariables simSettings;
    simSettings.playingfieldNumber = 2;
    simSettings.genomeSize = 50;

    simulation x(theMainWindow, 0, &simSettings);

    for (auto pf : x.playingFields)
        for (auto o : pf->playingField)
            o->initialise(simSettings.genomeSize);

    x.iterations = 66;

    theMainWindow->resizeGrid(simSettings);

    QVector <bool> genome (50, false);
    x.playingFields[0]->playingField[10]->speciesID = 1;
    x.playingFields[0]->playingField[10]->setGenome(genome);
    x.playingFields[1]->playingField[6]->speciesID = 1;
    x.playingFields[1]->playingField[6]->setGenome(genome);

    QHash<QString, QVector <int> > extinct = x.checkForExtinct(speciesList);

    for (auto s : extinct) x.speciesExtinction(speciesList[s[0]], x.playingFields[s[1]]->playingField[s[2]], (x.iterations + 1), simSettings.sansomianSpeciation);

    for (auto o : speciesList)
        if (o->extinct != 0)
        {
            testFlag = false;
            out << "Set species extinction to 66 for species 1 when there are two instances - fail.\n\n";
            return testFlag;
        }
    if (testFlag) out << "Nothing extinct in first part of test - passed.\nNext test should have two extinct taxa.\n";

    x.playingFields[1]->playingField[6]->speciesID = 2;

    extinct = x.checkForExtinct(speciesList);

    for (auto s : extinct) out << "Species is extinct: " << s[0] << "\n";
    for (auto s : extinct) x.speciesExtinction(speciesList[s[0]], x.playingFields[s[1]]->playingField[s[2]], (x.iterations + 1), simSettings.sansomianSpeciation);
    for (auto o : speciesList)
        if (o->extinct != 0)
        {
            out << "Extinct at " << o->extinct << "\nGenome should be all zeros. It is: ";
            for (auto b : o->genome)
            {
                out << (b ? "1" : "0");
                if (b)
                {
                    testFlag = false;
                    out << "Genome is incorrect in test 13.\n";
                }
            }
            if (o->extinct != 67)
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
    for (auto s : extinct) out << "Species is extinct: " << s[0] << "\n";
    for (auto s : extinct) x.speciesExtinction(speciesList[s[0]], x.playingFields[s[1]]->playingField[s[2]], (x.iterations + 1), simSettings.sansomianSpeciation);
    for (auto o : speciesList)
        if (o->extinct == 69)
        {
            out << "Extinct at " << o->extinct << "\nGenome should not be all zeros. It is: ";
            int count = 0;
            for (auto b : o->genome)
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
    theMainWindow->resizeGrid(simSettingsReset);

    return testFlag;
}

//Check count difference to parent
bool test::testFourteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Testing difference to parent.\n\n";

    //Initialised to false
    Organism org(50);
    for (auto &p : org.parentGenome) p = true;

    simulationVariables simSettings;
    simulation x(theMainWindow, 0, &simSettings);

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
bool test::testFifteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);
    out << "Check print matrix.\n\n";

    QVector <Organism *> speciesList;
    for (int i = 0; i < 20; i ++)
    {
        Organism *org = new Organism(50);
        speciesList.append(org);
    }
    simulationVariables simSettings;
    simulation x(theMainWindow, 0, &simSettings);

    out << "Should be all zeros.\n" << x.printMatrix(speciesList);

    return testFlag;
}

//Check print tree
bool test::testSixteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    out << "Check print tree.\n\n";

    QVector <Organism *> speciesList;
    for (int i = 0; i < 8; i ++)
    {
        Organism *org = new Organism(50);
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
    simulation x(theMainWindow, 0, &simSettings);
    QString tree(x.printNewickWithBranchLengths(0, speciesList, true, 8));
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

bool test::testSeventeen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testEighteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testNineteen(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}

bool test::testTwenty(QString &outString)
{
    bool testFlag = true;
    QTextStream out(&outString);

    return testFlag;
}
