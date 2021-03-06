#include "vtakey.h"
#include "globlevar.h"
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QtDebug>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QtGui>
#include <QAbstractItemModel>
#include <QClipboard>
#include <windows.h>


VtaKey::VtaKey(QWidget *parent)
: QLineEdit(parent), c(0)
{
    setCompleter();

    QFont font;
    font.setFamily(QStringLiteral("Times New Roman"));
    font.setPointSize(11);
    VtaKey::setFont(font);

    Qt::WindowFlags fla = this->windowFlags();
    this->setWindowFlags(fla | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    this->setGeometry(23,45,250,30);
    this->activateWindow();

    VtaKeyThread = new GetActiveWindowT("VtaKey");
    VtaKeyThread->start();

  //  EnglishList<<"this"<<"is"<<"my"<<"deep";
  //  HindiWord<<"दिस"<< "हे"<<"मेरा" <<"गहरा";


}
VtaKey::~VtaKey()
{

}
QAbstractItemModel *VtaKey::modelFromFile(const QString& fileName,const QString& NewStr)
{
       QFile file;

    if(isfrist)
    {
         file.setFileName(fileName);
        if (!file.open(QFile::ReadOnly))
            return new QStringListModel(c);
    }


#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif

    if(isfrist)
    {
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            if (!line.isEmpty())
                wordList << line.trimmed();
        }

    }
    else
    {
          wordList.removeAt(0);
          wordList.insert(0,NewStr); //<<NewStr;
          if(!Tempist.isEmpty())
          {
              wordList.removeAt(1);
              wordList.insert(1,Tempist);
          /*    QFile f("a.txt");
              f.open(QIODevice::WriteOnly);
              QTextStream t(&f);
              t.setCodec("UTF-8");
              t<<wordList.at(1); */
          }
    }



#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(wordList, c);
}

void VtaKey::setCompleter()
{
    isfrist = true;

     c = new QCompleter(this);
     c->setModel(modelFromFile("VtaFiles//HiHindiWordList.Vta",""));
     c->setWrapAround(false);

  isfrist  = false;

    if (c)
        QObject::disconnect(c, 0, this, 0);

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(c, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));



}
QCompleter *VtaKey::completer() const
{
    return c;
}
int VtaKey::SendText()
{

    SetForegroundWindow(AcWindowId);
     // Create a generic keyboard event structure
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;



               // Press the "Ctrl" key
                ip.ki.wVk = VK_CONTROL;
                ip.ki.dwFlags = 0; // 0 for key press
                SendInput(1, &ip, sizeof(INPUT));

                // Press the "V" key
                ip.ki.wVk = 'V';
                ip.ki.dwFlags = 0; // 0 for key press
                SendInput(1, &ip, sizeof(INPUT));

                // Release the "V" key
                ip.ki.wVk = 'V';
                ip.ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(1, &ip, sizeof(INPUT));

                // Release the "Ctrl" key
                ip.ki.wVk = VK_CONTROL;
                ip.ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(1, &ip, sizeof(INPUT));

      Sleep(50);

  //    HWND d =  GetForegroundWindow();
  //     qDebug()<<d;

   //     SetForegroundWindow(d);

    //  qDebug()<<qApp->activeWindow();
    //  qDebug()<<this->isActiveWindow();


}
void VtaKey::insertCompletion(const QString& completion)
{

    char ti[256];
      GetWindowText(AcWindowId,(LPWSTR)ti,sizeof(ti));
     setWindowTitle("AW - "+QString::fromLatin1(ti));

     // setText(completion);
   // QLineEdit::clear();
      QClipboard *clipboard = QApplication::clipboard();
      QString originalText = clipboard->text();
      clipboard->clear();

     clipboard->setText(completion+" ");

      SendText();

     this->activateWindow();

     QLineEdit::clear();
    // clipboard->setText(originalText);


/*
   HWND hw = GetForegroundWindow();
   char ti[256];
   GetWindowText(hw,(LPWSTR)ti,sizeof(ti));
   qDebug()<<ti;

   HWND hNotepad = FindWindow(TEXT("v"), NULL);
   if (!hNotepad)
      qDebug()<< "Notepad was not found!\n";
   else
   {
       HWND hEdit = FindWindowEx(hNotepad, NULL, TEXT("Edit"), NULL);

       if (!hEdit)
            qDebug()<< "Notepads edit control was not found!\n";
       else
       {
           SetForegroundWindow(hNotepad);
       }
   }
  */

}

void VtaKey::focusInEvent(QFocusEvent *e)
{
    if (c)
        c->setWidget(this);
    QLineEdit::focusInEvent(e);
}
QString VtaKey::textUnderCursor()
{
    Tempist = "";
      QString t  = Vta_En_to_Hi(text());

   for(int i=0;i<EnglishList.length();i++)
   {
       if(text()  == EnglishList.at(i))
       {
           Tempist = HindiWord.at(i);
           break;
       }
   }

      c->setModel(modelFromFile("VtaFiles//HiHindiWordList.Vta",t));

      return t;
}
void VtaKey::SetString(const QString& str)
{
    //PopedString = str;
   // wordList<<str;
}
void VtaKey::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
              e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }


    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QLineEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 1
                      || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix())
    {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
     cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!

}
QString VtaKey::Vta_En_to_Hi(QString str)
{

             QString s =  str;
             QStringList MatraT,MatraHT,MatraHLT,MatraLT,HiHaT,HiHaHT;

             MatraT<<"n:"<<"aa"<<"ee"<<"e"<< "au"<<"ai"<<"oo"<<"o"<<"i"<<"a"<<"u";
             MatraHT<<"ं"<<"1ा" <<"1ी" <<"1े" <<"1ौ" <<"1ै" <<"1ू" <<"1ो"<<"1ि"<<"1"<<"1ु";

             MatraHLT<<"1ा" <<"1े"<<"1ी" <<"1ो"<<"1ौ" <<"1ै"<<"1ू" <<"1ि"<<"1ु"<<"न्:"<<"1";
             MatraLT<<"आ"<<"ए"<<"ई"<<"ओ"<<"औ"<<"ऐ"<<"ऊ"<<"इ"<<"उ"<<"ं"<<"अ";

             HiHaT<<"gy"<<"ksh"<<"N"<<"dh"<< "kh"<<"k"<<"gh"<<"n~"<<"n"<<"chh"<<"ch"<<"jh"<<"j"<<"th"<<"t:"<<"t"<<
                  "dh"<<"d"<<"ph"<<"p"<< "bh"<<"b"<<"m"<<"y"<<"r"<<"l"<<"v"<<"Sh"<<"sh"<<"s"<<"h"<<"g"<<"f"<<"z";
             HiHaHT<<"ज्ञ्"<<"क्ष्"<<"ण्"<<"ढ्"<<"ख्"<<"क्"<<"घ्"<<"ङ्"<<"न्"<<"छ्"<<"च्"<<"झ्"<<"ज्"<<"थ्"<<"ट्"<< "त्"<<
                   "ध्"<<"द्"<<"फ्"<<"प्"<<"भ्"<<"ब्"<<"म्"<<"य्"<<"र्"<<"ल्"<<"व्"<<"ष्"<<"श्"<<"स्"<<"ह्"<<"ग्"<<"फ्"<<"ज्";



             for(int fc = 0;fc<HiHaT.length();fc++)
            {
                    s  =  s.replace(HiHaT[fc],HiHaHT[fc],Qt::CaseInsensitive);
            }

            for(int ma = 0;ma<MatraT.length();ma++)
             {
                   s  =  s.replace(MatraT[ma],MatraHT[ma],Qt::CaseInsensitive);
             }
                   s  =  s.replace("्1", "",Qt::CaseInsensitive);

            for(int ma = 0;ma<MatraLT.length();ma++)
             {
                 s  =  s.replace(MatraHLT[ma],MatraLT[ma],Qt::CaseInsensitive);
             }

             return s;

}
