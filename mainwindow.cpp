#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createdefaultTexts();

    this->setMinimumSize(900, 500);
    view = new QGraphicsView();
    this->setCentralWidget(view);

    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, this->width() - 50, this->height() - 50);
    view->setScene(scene);

    QFont textFont("Helvetica [Cronyx]", 20);
    text = scene->addText(defaultTexts[8], textFont);
    QStringList words = defaultTexts[8].split(" ");
    countWords = words.size();
    countChars = defaultTexts[8].size();
    startCursor();

    timer = new QTimer();
    timer->start(100);
    createStatusBar();


    createKeyboard("ru");
}

MainWindow::~MainWindow() {}

void MainWindow::createStatusBar()
{
    languages = new QComboBox(this->statusBar());
    restartButton = new QPushButton("Обновить");
    timeLabel = new QLabel("Время: 0");
    charPerSec = new QLabel("CPS: 0");
    worldPerSec = new QLabel("WPS: 0");
    accuracu = new QLabel("Аккуратность: 100%");
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::generateText);
    // connect(timer, &QTimer::timeout, this, &MainWindow::updateTimeLabel);
    languages->addItems({"Английский", "Арабский", "Белорусский", "Испанский", "Итальянский", "Немецкий", "Польский", "Португальский", "Русский", "Французский"});
    // {"us", "ar", "by", "es", "it", "de", "pl", "pt", "ru", "fr"};
    languages->setGeometry(scene->width() - 100, scene->height() - 100, languages->width(), languages->height());
    this->statusBar()->addWidget(languages);
    this->statusBar()->addWidget(restartButton);
    this->statusBar()->addWidget(timeLabel);
    this->statusBar()->addWidget(charPerSec);
    this->statusBar()->addWidget(worldPerSec);
    this->statusBar()->addWidget(accuracu);
    this->statusBar()->setFocusPolicy(Qt::NoFocus);
    languages->setFocusPolicy(Qt::NoFocus);
    restartButton->setFocusPolicy(Qt::NoFocus);
}

void MainWindow::nextLetter(QKeyEvent *key)
{
    if(!timer->isActive())
        return;

    int keyCode = key->nativeScanCode();
    if(keyCode >= 24 && keyCode <= 35)
    {
        buttons[keyCode - 24]->pressOn();
    }
    else if(keyCode >= 38 && keyCode <= 48)
    {
        buttons[keyCode - 26]->pressOn();
    }
    else if(keyCode >= 52 && keyCode <= 61)
    {
         buttons[keyCode - 29]->pressOn();
    }

    QTextCharFormat format;
    if(key->key() == Qt::Key_Backspace)
    {
        --totalInputCh;
        format.setUnderlineStyle(QTextCharFormat::NoUnderline);
        cursor->mergeCharFormat(format);
        cursor->movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, 2);
        cursor->movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

        format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        format.setForeground(Qt::gray);
        cursor->mergeCharFormat(format);
        return;
    }

    if(key->text().isEmpty())
        return;

    if(key->text().at(0).isLetter() || key->text().at(0) == " " || key->text().at(0) == "-")
    {
        ++inputCh;
        if(statedWrite == 0)
        {
            connect(timer, &QTimer::timeout, this, &MainWindow::updateTimeLabel);
            deltaTime = QTime::currentTime();
            statedWrite = 1;
        }

        if(key->text() == cursor->selectedText())
        {
            format.setUnderlineStyle(QTextCharFormat::NoUnderline);
            format.setForeground(Qt::white);
            cursor->mergeCharFormat(format);
            cursor->movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);

            cursor->movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
            format.setUnderlineColor(QColor("aqua"));
            format.setForeground(Qt::gray);
            cursor->mergeCharFormat(format);

            ++totalInputCh;
            ++ch;
            if (cursor->selectedText() == " ") {
                ++wr;
                --ch;
            }
        }
        else
        {
            if(cursor->selectedText() == " ")
            {
                return;
            }
            format.setUnderlineStyle(QTextCharFormat::NoUnderline);
            format.setForeground(Qt::red);
            cursor->mergeCharFormat(format);
            cursor->movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);

            cursor->movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
            format.setUnderlineColor(QColor("aqua"));
            format.setForeground(Qt::gray);
            cursor->mergeCharFormat(format);
            ++totalInputCh;
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    nextLetter(event);
}

void MainWindow::generateText()
{
    disconnect(timer, &QTimer::timeout, this, &MainWindow::updateTimeLabel);

    ch = 0;
    wr = 0;
    inputCh = 0;
    totalInputCh = 0;
    statedWrite = 0;
    timeLabel->setText("Время: 0");
    charPerSec->setText("CPS: 0");
    worldPerSec->setText("WPS: 0");
    accuracu->setText("Аккуратность: 100%");

    if(completeReply == 0)
        return;
    completeReply = 0;
    QString language = languages->currentText();

    QJsonObject json;
    json["model"] =
        "gpt-4o-mini";
    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";
    message["content"] = "Напиши разный текст на случайную тему, используя " + language + " язык, для клавиатурного тренажера на 25 слов, начни писать сразу текст, без вводных слов, и не нужно использовать никакие знаки препинания";
    messages.append(message);
    json["messages"] = messages;
    json["max_tokens"] = 100;
    QJsonDocument doc(json);

    QNetworkRequest request;
    request.setUrl(QUrl("https://api.aimlapi.com/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader(
        "Authorization",
        "Bearer 4640f714b18c4a7e838b5fc323a74984");


    reply = manager.post(request, doc.toJson());
    QObject::connect(reply, &QNetworkReply::finished, this, &MainWindow::updateText);

    timer->start(100);
    qInfo() << countChars << " | " << countWords;
}

void MainWindow::updateText()
{
    QList<std::string> langList = {"us", "ara", "by", "es", "it", "de", "pl", "pt", "ru", "fr"};
    for(int i = 0; i < buttons.size(); i++)
    {
        scene->removeItem(buttons[i]);
    }
    buttons.clear();
    createKeyboard(langList[languages->currentIndex()]);
    scene->update();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument responseDoc = QJsonDocument::fromJson(response);
        QJsonObject responseObj = responseDoc.object();
        QString content = responseObj["choices"]
                              .toArray()[0]
                              .toObject()["message"]
                              .toObject()["content"]
                              .toString();
        content.front() = content.front().toUpper();
        QStringList words = content.split(" ");
        countWords = words.size();
        countChars = content.size();
        text->setPlainText(content);
        qInfo() << content;
    } else {
        text->setPlainText("К сожалению гптшка накрылась вот скучный и однообразный текст\n\n" + defaultTexts.at(languages->currentIndex()));
        QStringList words = defaultTexts.at(languages->currentIndex()).split(" ");
        countWords = words.size();
        countChars = defaultTexts.at(languages->currentIndex()).size();
    }
    startCursor();
    reply->deleteLater();
    completeReply = 1;
}

void MainWindow::updateTimeLabel()
{
    if (totalInputCh == countChars) {
        timer->stop();
    }

    timeLabel->setText("Время: " + QString::number((double)deltaTime.msecsTo(QTime::currentTime()) / 1000, 'g', 3));
    charPerSec->setText("CPS: " + QString::number((double)ch / ((double)deltaTime.msecsTo(QTime::currentTime()) / 1000), 'g', 3));
    worldPerSec->setText("WPS: " + QString::number( (double)(wr + 1) / ((double)deltaTime.msecsTo(QTime::currentTime()) / 1000), 'g', 3));
    accuracu->setText("Аккуратность: " + QString::number((ch + wr) * 100 / inputCh) + "%");
}

void MainWindow::startCursor()
{
    QTextDocument *doc = text->document();
    doc->setPageSize(QSizeF(scene->width(), scene->height()));
    cursor = new QTextCursor(doc);
    QTextCharFormat defaultFormat;
    defaultFormat.setForeground(Qt::gray);

    cursor->select(QTextCursor::Document);
    cursor->mergeCharFormat(defaultFormat);
    QTextBlockFormat blockFormat;
    blockFormat.setLeftMargin((scene->width() - 800) / 2);
    blockFormat.setRightMargin((scene->width() - 800) / 2);
    cursor->mergeBlockFormat(blockFormat);

    cursor->setPosition(0);
    cursor->select(QTextCursor::SelectionType::BlockUnderCursor);
    if(cursor->selectedText() == "К сожалению гптшка накрылась вот скучный и однообразный текст")
    {
        QTextCharFormat errorText;
        errorText.setForeground(Qt::white);
        errorText.setFont(QFont("Helvetica [Cronyx]", 17));
        // errorText.setAnchor(1);
        cursor->mergeCharFormat(errorText);
        cursor->setPosition(cursor->position() + 2);
    }
    else
    {
        cursor->setPosition(0);
    }
    cursor->movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    QTextCharFormat format;
    format.setUnderlineColor(QColor("aqua"));
    format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
    cursor->mergeCharFormat(format);
}

void MainWindow::createdefaultTexts()
{
    defaultTexts.push_back("The sun rises every morning awakening the world with its beautiful light nature comes alive with colors and sounds birds chirp and flowers bloom in harmony");
    defaultTexts.push_back("الشمس تشرق في الصباح وتضيء السماء باللون الأزرق الطيور تغرد والأشجار تتمايل مع النسيم الأصدقاء يجتمعون في الحديقة للعب والاستمتاع بالوقت الممتع");
    defaultTexts.push_back("Вясна прынесла новыя надзеі дзяўчаты гуляюць у парку людзі займаюцца спортам ўся прырода напаўняецца жыццём і радасцю сонца свеціць ярка над зямлёй");
    defaultTexts.push_back("El cielo azul brilla sobre la ciudad llena de vida los niños juegan en el parque mientras las flores florecen y los pájaros cantan felices por la mañana");
    defaultTexts.push_back("La primavera è una stagione meravigliosa con fiori colorati che sbocciano gli uccelli cantano e il sole riscalda laria creando unatmosfera di gioia e rinascita");
    defaultTexts.push_back("Die Sonne scheint heute hell am Himmel Kinder spielen im Park Vögel singen fröhlich die Blumen blühen bunt und die Luft riecht nach Frühling");
    defaultTexts.push_back("W moim mieście znajduje się piękny park gdzie można spacerować biegać i odpoczywać wśród drzew oraz cieszyć się świeżym powietrzem z przyjaciółmi podczas pikniku");
    defaultTexts.push_back("A natureza é incrível cheia de cores e sons podemos explorar florestas montanhas e rios cada lugar tem sua beleza única que nos encanta sempre");
    defaultTexts.push_back("Солнце светит ярко на небе птицы поют в траве щебечут цветы распускаются весной наступает радость жизни и счастье наполняет сердца людей и животных");
    defaultTexts.push_back("Les chats sont des animaux fascinants ils aiment jouer courir et explorer leur environnement ils sont aussi très affectueux et apportent de la joie à leurs propriétaires");
    defaultTexts.push_back("");
}

void MainWindow::createKeyboard(std::string lan)
{
    xkb_context *ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if (!ctx) qInfo() << "Error";

    struct xkb_keymap *keymap;
    struct xkb_rule_names names = {
        .rules = NULL,
        .model = "pc105",
        .layout = lan.c_str(),
        .variant = NULL,
        .options = ""
    };
    keymap = xkb_keymap_new_from_names(ctx, &names,
                                       XKB_KEYMAP_COMPILE_NO_FLAGS);
    if (!keymap) qInfo() << "Error";

    struct xkb_state *state;
    state = xkb_state_new(keymap);
    if (!state) qInfo() << "Error";

    QList<QChar> listChar;
    for(xkb_keycode_t i = 24; i < 256; i++)
    {
        xkb_keysym_t keysym = xkb_state_key_get_one_sym(state, i);
        char buffer[32];
        int size = xkb_keysym_to_utf8(keysym, buffer, sizeof(buffer));
        if(size > 1)
        {
            listChar.push_back(QString::fromUtf8(buffer).at(0).toUpper());
        }
    }

    for(int i = 0; i < 12; i++)
    {
        buttons.push_back(new KeyButton(scene, QRectF(0 + i * 55, 0, 50, 50), listChar[i]));
        scene->addItem(buttons.back());

    }
    for(int i = 0; i < 11; i++)
    {
        buttons.push_back(new KeyButton(scene, QRectF(25 + i * 55, 55, 50, 50), listChar[i + 13]));
        scene->addItem(buttons.back());
    }
    for(int i = 0; i < 10; i++)
    {
        buttons.push_back(new KeyButton(scene, QRectF(50 + i * 55, 110, 50, 50), listChar[i + 26]));
        scene->addItem(buttons.back());
    }
}

