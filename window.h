#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QtUiTools/QtUiTools>
#include <fstream>
namespace Ui {
class window;
}

class window : public QMainWindow
{
    Q_OBJECT


public:
    explicit window(QWidget *parent = nullptr);
    ~window();

    enum Nam { HAM = 0, SPAM = 1 };
    enum MODE{DEFAULT= 0, DEBUG, LDEBUG};


    std::fstream Stream;

    QTimer* stopWatch;
    struct wordStats
    {
    public:

        wordStats(std::string call, Nam Status ) : name(call) //sets the name to the name you gave it in the parameter
        {
            if (Status == SPAM)
            {
                PlusSpam();
            }
            else if (Status == HAM)
            {
                PlusHam();
            }
            Plus();
        }
        void PlusSpam()
        {
            spamCount += 1;
        }
        void PlusHam()
        {
            hamCount += 1;
        }
        void Plus()
        {
            total += 1;
        }

        unsigned int DisplaySpamCount()
        {
            return spamCount;
        }
        unsigned int DisplayHamCount()
        {
            return hamCount;
        }
        unsigned int DisplayTotal()
        {
            return total;
        }
        std::string DisplayName()
        {
            return name;
        }
        private:
        std::string name;
        unsigned int total = 0; //total times the word has been counted;
        unsigned int spamCount = 0;
        unsigned int hamCount = 0;
    };


    std::string inline removeCharInString(std::string& String, char a);
    std::vector<std::string> inline ReturnWords(std::string& message, MODE mode);
    //-------------------------------------------------------------------------------------------------
    std::vector<wordStats> inline ReturnUniqueWordsClasses(std::vector<std::string> Vector, std::vector<wordStats> wordList, MODE mode);
    void inline printClassVector(std::vector<wordStats> Vector);
    wordStats inline ReturnWordStats(std::string name, std::vector<wordStats>& wordList, MODE mode  = DEFAULT);
    //-------------------------------------------------------------------------------------------------
    double inline ReturnSpamProduct(std::vector<std::string> StringVector, std::vector<wordStats>& WordList);
    double inline ReturnHamProduct(std::vector<std::string> StringVector, std::vector<wordStats>& WordList);
    //-------------------------------------------------------------------------------------------------
    double inline ReturnProb(std::string message, std::vector<wordStats>& wordList, MODE mode );


    std::vector<wordStats> WordList;
    std::vector<wordStats>* ListPtr = &WordList;

public Q_SLOTS:
    void on_label_windowIconTextChanged();



signals:
    inline void textChanged();

private slots:
  void on_textEdit_textChanged();

void on_actionInfo_triggered();
void setLabelText();
void on_actionCopy_triggered();

void on_actionCut_triggered();

void on_actionOpen_Working_Directory_triggered();

void on_actionReset_Training_Data_triggered();

private:
    Ui::window *ui;
    protected:
};









#endif // WINDOW_H
