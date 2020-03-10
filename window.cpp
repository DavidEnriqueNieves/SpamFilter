#include "window.h"
#include "ui_window.h"
#include <QElapsedTimer>
#include <QTimer>
#include <QFile>
#include <QElapsedTimer>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <QIcon>

window::window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::window)
{

   QShortcut* shortcut  = new QShortcut(QKeySequence(tr("Ctr+O", "Import CSV")), this);

    ui->setupUi(this);
    stopWatch = new QTimer(this);
    stopWatch ->setSingleShot(true);
    QIcon Icon("spam.ico");
    window::setWindowIcon(Icon);

    QFile file("spam.csv");
    WordList.emplace_back(wordStats("spam", SPAM)); //make sure to subtract one from the spamcount
    WordList.emplace_back(wordStats("ham", HAM));// same for the hamcount
    //connect shortcut to open csv
   // QObject::connect(this, SIGNAL(shortcut->activated()), this, SLOT(on_actionOpen_Working_Directory_triggered()));

    window::Stream.open("spam.csv");
    std::string line;
    if(Stream.fail())
    {
        QMessageBox::information(this,"..","\nThe file spam.csv has not been found. Make sure it is in the .exe directory!");
        QString file_name2 = QFileDialog::getOpenFileName(this, "Open a file", "debug");
    }
    else
    {
        while(getline(Stream, line))
        {
          WordList = ReturnUniqueWordsClasses(ReturnWords(line, DEFAULT), WordList, DEFAULT);
        }
    }
    qDebug()<<"Done with WordList";
    connect(this->stopWatch, SIGNAL(timeout()), this, SLOT(setLabelText()));

}


window::~window()
{
    delete ui;
}



void window::on_textEdit_textChanged()
{
 qDebug()<<"\nEmitting on_textEdit_textChanged()";
emit textChanged();

}



void window::on_label_windowIconTextChanged()
{
    qDebug()<<"\nHEEEELP!";
    emit window::textChanged();
}

void window::on_actionInfo_triggered()
{

    //IMPORTANT, MAKE IT SO THAT THE USER CAN COPY THE URL FROM THE WINDOW


     QString info  = QString::fromStdString("This is an App to test whether your "
                                             "message is spam\nor not spam. In this case, we refer to "
                                            "things that aren't spam  as \"ham\". \n"
                                          "To add training data. make sure your file is a comma "
                                        "separated list, with the first keyword "
                                      "being either spam or ham, a comma, the message, and three commas"
                                    " to separate the next message"
                                            "\n\nFor reference on how to compose your training data CSV's, please go to the .exe"
                                            "\ndirectory and take a look at the spam.csv file provided by kaggle.com"
                                            "\nhttps://www.kaggle.com/uciml/sms-spam-collection-dataset"
                                            "<a href='https://www.kaggle.com/uciml/sms-spam-collection-dataset'</a>"
                                            "\nIcon is from http://www.iconarchive.com/show/grocery-icons-by-pixture/Spam-icon.html"
                                       );
   QMessageBox n;
   n.setText(info);
   n.setTextFormat(Qt::RichText);
   n.exec();
}

void window::textChanged()
{
    qDebug()<<"\nEmitting Text Changed!";
    stopWatch->start(500);
}







void window::printClassVector(std::vector<wordStats> Vector)
{
    for (unsigned int i = 0; i < static_cast<unsigned int>(Vector.size()); i++)
    {
        std::cout << "\nNAME: "<<Vector[i].DisplayName();
        std::cout << "\nTOTAL: " << Vector[i].DisplayTotal();
    }
}





std::vector<std::string> inline window::ReturnWords(std::string& message, MODE mode)
{
    std::vector<std::string>words;
    std::string cstring = message;

    if (cstring.substr(0, 5) == "spam,")
    {
        cstring.erase(0, 5);
        words.emplace_back("spam");

    }
    else if (cstring.substr(0, 4) == "ham,")
    {
        cstring.erase(0, 4);
        words.emplace_back("ham");

    }
    for (unsigned int i = 0; i < static_cast<unsigned>(cstring.length()); i++) // Go through the length of the message
    {
        if (cstring[i] == ' ') //if it finds a space, scan the sentence backwards until it finds another space
        {                      //and cut the words in between
            for (int u = static_cast<int>(i) - 1; u >= 0; u--)
            {
                if (u == 0)
                {
                    std::string copy = cstring.substr(static_cast<unsigned int>(u), i - static_cast<unsigned int>(u));
                    copy = removeCharInString(copy, '!');
                    copy = removeCharInString(copy, '?');
                    copy = removeCharInString(copy, '(');
                    copy = removeCharInString(copy, ')');
                    copy = removeCharInString(copy, '=');
                    copy = removeCharInString(copy, '\"');
                    copy = removeCharInString(copy, '.');
                    copy = removeCharInString(copy, ' ');
                    if (copy != "") words.emplace_back(copy);

                    break;
                }
                if (cstring[static_cast<unsigned int>(u)] == ' ')
                {
                    std::string copy = cstring.substr(static_cast<unsigned int>(u)+1, i - static_cast<unsigned int>(u));
                    copy = removeCharInString(copy, '!');
                    copy = removeCharInString(copy, '?');
                    copy = removeCharInString(copy, '(');
                    copy = removeCharInString(copy, ')');
                    copy = removeCharInString(copy, '=');
                    copy = removeCharInString(copy, ',');
                    copy = removeCharInString(copy, '\"');
                    copy = removeCharInString(copy, '.');
                    copy = removeCharInString(copy, ' ');
                    if (copy != "") words.emplace_back(copy);
                    break;
                }


            }
        }

    }
    /*if the last character in the sentece is a " ", don't add it, however,
        if the last instance of' ' has a word right in front, add that word*/
    if (cstring.find_last_of(' ') != cstring.length())
    {
        std::string copy  = cstring.substr(cstring.find_last_of(' ') + 1, cstring.length() - cstring.find_last_of(' '));
        copy = removeCharInString(copy, '!');
        copy = removeCharInString(copy, '?');
        copy = removeCharInString(copy, '(');
        copy = removeCharInString(copy, ')');
        copy = removeCharInString(copy, '=');
        copy = removeCharInString(copy, ',');
        copy = removeCharInString(copy, '\"');
        copy = removeCharInString(copy, '.');
        copy = removeCharInString(copy, ' ');

        if (copy != "") words.emplace_back(copy);
    }
    if (mode == DEBUG)
    {
        qDebug()<<"Printing Words!";
    }
    return words;
}

std::vector<window::wordStats> inline window::ReturnUniqueWordsClasses(std::vector<std::string> Vector, std::vector<wordStats> wordList, MODE mode = DEFAULT)
{
    if (mode == DEBUG)
    {
        std::cout << "\nStarting the Return Unique Words Classes function!";
        std::cout << "\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-";
    }
    unsigned int newWords = 0;

    if (Vector[0] == "spam")
    {
        wordList[0].Plus();
    }
    else if (Vector[0] == "ham")
    {
        wordList[1].Plus();

    }
    for (unsigned int i = 0; i < static_cast<unsigned int>(Vector.size()); i++) //Go through each word in the list of words
    {
        unsigned int index = 0;
        bool wordFound = false;
        if (mode == DEBUG)
        {
            std::cout << "\n=============================================";
            std::cout << "\n\n ANALYZING THE WORD " << Vector[i];
            std::cout << "\nchecking if the word is already in the wordlist...";
        }
        for (int u = 0; u < int(wordList.size()); u++) // checks to see if the word is already registered in the wordlist
        {
            if (Vector[i] == wordList[static_cast<unsigned int>(u)].DisplayName())
            {
                //std::cout << "\nWord Found!";
                index = static_cast<unsigned int>(u);
                wordFound = true;
                //std::cout <<"\n"<< wordList[u].name;

            }
        }
        if ((wordFound = true) && (wordList[index].DisplayName()!="spam")&&(wordList[index].DisplayName()!="ham"))
        {
            if (Vector[0] == "spam")
            {
                wordList[index].PlusSpam();
                wordList[0].PlusSpam();
            }
            else if (Vector[0] == "ham")
            {
                wordList[index].PlusHam();
                wordList[1].PlusHam();
            }
            if (mode == DEBUG)
            {
                std::cout << "\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
                std::cout << "\n Found a word. Word is " << wordList[index].DisplayName();
                std::cout << "\n Word's " << (Vector[0] == "spam" ? "spam" : "ham") << "counter will be increased!";
            }
            wordList[index].Plus();
            if (mode == DEBUG)
            {
                std::cout << "\nTOTAL IS " << wordList[index].DisplayTotal();
                std::cout << "\nSPAM COUNTER IS " << wordList[index].DisplaySpamCount();
                std::cout << "\nHAM COUNTER IS " << wordList[index].DisplayHamCount();
                std::cout << "\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n";
            }

        }
        else if((Vector[i]!= "spam") && (Vector[i]!= "ham"))
        {
            if (mode == DEBUG)
            {
                std::cout << "\n Word not found! Creating a new WordClass for " << Vector[i];
                std::cout << "\n Adding a " << (Vector[0] == "spam" ? "spam" : "ham") << "counter for the word";
            }

            newWords += 1;
            if (Vector[0] == "spam")
            {
                wordList.emplace_back(wordStats(Vector[i], SPAM));// add a new class to the wordList with the word and the type of word it is
                wordList[0].PlusSpam();
            }
            else if (Vector[0] == "ham")
            {
                wordList.emplace_back(wordStats(Vector[i], HAM));
                wordList[1].PlusHam();
            }
            else std::cout << "\n ERROR, WORD FOUND IN SENTENCE THAT IS NEITHER SPAM NOR HAM!!";
        }
        else if(mode==DEBUG) std::cout << "\nWord is " << (Vector[0] == "spam" ? "spam" : "ham") << "This does not count!";
    }
    if (mode == DEBUG)
    {
        std::cout << "\nNEW WORDS: " << newWords;
    }
    if (mode == DEBUG || mode == LDEBUG)
    {
        for (unsigned int y = 0; y < static_cast<unsigned int>(wordList.size()); y++)
        {
            std::cout << "\n------------------------------";
            std::cout << "\nNAME " << wordList[y].DisplayName();
            std::cout << "\nTOTAL " << wordList[y].DisplayTotal();
            std::cout << "\nHAMCOUNT " << wordList[y].DisplayTotal();
            std::cout << "\nSPAMCOUNT " << wordList[y].DisplayTotal();
        }
    }
    return wordList;
}





std::string inline window::removeCharInString(std::string& String, char a)
{
    while (String.find(a) != std::string::npos)
    {
        String.erase(String.find(a), 1);
    }
    return String;
}




window::wordStats inline window::ReturnWordStats(std::string name, std::vector<wordStats>& wordList, MODE mode)
{

    bool found = true;
    for (unsigned int i = 0; i < static_cast<unsigned int>(wordList.size()); i++)
    {
        if (name == wordList[i].DisplayName())
        {
            if (mode == DEBUG) std::cout << "\nFOUND THE WORDCLASS!";
            std::cout << "Word has " << wordList[i].DisplayHamCount()<<" ham entries and " << wordList[i].DisplaySpamCount() << " spam entries";
            if (wordList[i].DisplaySpamCount() == 0)
            {
                wordList[i].PlusSpam();
            }
            if (wordList[i].DisplayHamCount() == 0)
            {

                wordList[i].PlusHam();
            }

            return wordList[i];

        }
    }
    if (found != true)
    {
        wordStats NewSpecs = wordStats(name, HAM); //CHANGE THIS POSSIBLY
        NewSpecs.PlusSpam();
        return NewSpecs;
    }
}

double window::ReturnProb(std::string message,std::vector<wordStats>& wordList , MODE mode  = DEFAULT)
{
    qDebug()<<ReturnHamProduct(ReturnWords(message, mode), wordList)/ReturnSpamProduct(ReturnWords(message, mode), wordList);

    //WHY IS THIS 0
    double Result = log10((double(wordList[0].DisplayTotal())/double(wordList[1].DisplayTotal()))*(ReturnSpamProduct(ReturnWords(message, mode), wordList)/ ReturnHamProduct(ReturnWords(message, mode), wordList)));
    return Result;
}


double window::ReturnSpamProduct(std::vector<std::string> StringVector, std::vector<wordStats>& WordList)
{
    double Product  = 1.0;

    for (unsigned int i = 0; i<static_cast<unsigned int>(StringVector.size()); i++)
    {
        wordStats Stat = ReturnWordStats(StringVector[i], WordList, DEFAULT);
        //MAKE SURE THIS PRODUCT IS NEVER ZERO
        Product *= double(Stat.DisplaySpamCount())/double( WordList[0].DisplayTotal()-1);
    }
    return Product;
}


double window::ReturnHamProduct(std::vector<std::string> StringVector, std::vector<wordStats>& WordList)
{
    double Product = 1.0;

    for (unsigned int i = 0; i<static_cast<unsigned int>(StringVector.size()); i++)
    {

        wordStats Stat = ReturnWordStats(StringVector[i], WordList, DEFAULT);
        //MAKE SURE THIS PRODUCT IS NEVER ZERO

        Product *= double(Stat.DisplayHamCount()) / double(WordList[1].DisplayTotal() - 1);
    }
    return Product;
}


void window::setLabelText()
{
    QString  message  = ui->textEdit->toPlainText();
    std::string messageString   = message.toStdString();
    qDebug()<<ReturnProb(messageString, WordList, DEFAULT);
    double Prob = ReturnProb(messageString, WordList, DEFAULT);
    ui->label->setText(QString::fromStdString(std::to_string(Prob)+ ((Prob > 0.0)?"\nspam":"\nham")));
}







void window::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}

void window::on_actionCut_triggered()
{
    ui->textEdit->cut();
}

void window::on_actionOpen_Working_Directory_triggered()
{
bool fileFound = false;
QFileDialog* File;
     QUrl URL = File->getOpenFileUrl(this, "Open a file");
     QString filePath = QString::fromStdString(((URL.toString()).toStdString()).erase(0, 8));
     std::string fileString = ((URL.toString()).toStdString()).erase(0, 8);
     std::string file_ending = (fileString.substr(fileString.length()-3, 3));
     qDebug()<<QString::fromStdString(((URL.toString()).toStdString()).erase(0, 8));
    if(file_ending!="csv")
    {
        QMessageBox::about(this, "Warning", "Make sure you select a .csv file");

    }
    else fileFound = true;

    if(fileFound == true)
    {
        QFile file (filePath);
       if(file.open(QFile::ReadOnly|QIODevice::Text))
       {
         QTextStream out(&file);
         QString line;
         do
         {
             line = out.readLine();
             std::string h = line.toStdString();
             WordList = ReturnUniqueWordsClasses(ReturnWords(h, DEFAULT), WordList, DEFAULT);
             qDebug()<<line;
             qDebug()<<"\nLINE--------------";
         }while(!line.isNull()&&(!out.atEnd()));

            QMessageBox::about(this, "Success", "Successfully Parsed the .csv file");

        }
       file.close();
    }
}

void window::on_actionReset_Training_Data_triggered()
{

    window::Stream.open("spam.csv");
    std::string line;
    if(Stream.fail())
    {
        QMessageBox::information(this,"..","\nThe file spam.csv has not been found. Make sure it is in the .exe directory!");
        QString file_name2 = QFileDialog::getOpenFileName(this, "Open a file", "debug");
    }
    else
    {
        while(getline(Stream, line))
        {
          WordList = ReturnUniqueWordsClasses(ReturnWords(line, DEFAULT), WordList, DEFAULT);
        }
    }

}
