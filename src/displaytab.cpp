#include "displaytab.h"
#include <QDebug>

displayTab::displayTab(QString name, QWidget *parent, bool getVibrance) : QWidget(parent), name(name){
	makeTab();

	if(getVibrance){
		currentVibrance = getNvidiaSettingsVibrance(name);
	}
}

displayTab::displayTab(const displayTab &other) : QWidget (other.parentWidget()){
	makeTab();
	slider->setValue(other.slider->value());
	spinBox->setValue(other.spinBox->value());
	name = other.name;
	currentVibrance = other.currentVibrance;
}

displayTab::displayTab(displayTab &&other) noexcept{
	label = other.label;
	slider = other.slider;
	spinBox = other.spinBox;
	layout = other.layout;
	name = std::move(other.name);
	currentVibrance = other.currentVibrance;

	other.label = nullptr;
	other.slider = nullptr;
	other.spinBox = nullptr;
	other.layout = nullptr;
}

displayTab::~displayTab(){
	delete label;
	delete slider;
	delete spinBox;
	delete layout;
}

void displayTab::makeTab(){
	label = new (std::nothrow) QLabel("Vibrance", this);
	if(!label){
		throw std::runtime_error("failed to allocate memory for label");
	}

	slider = new (std::nothrow) QSlider(Qt::Orientation::Horizontal, this);
	if(!slider){
		delete label;
		throw std::runtime_error("failed to allocate memory for slider");
	}
    slider->setRange(0, 400);

	spinBox = new (std::nothrow) QSpinBox(this);
	if(!spinBox){
		delete label;
		delete slider;
		throw std::runtime_error("failed to allocate memory for spinbox");
	}
    spinBox->setRange(0, 400);

	layout = new (std::nothrow) QGridLayout();
	if(!layout){
		delete label;
		delete slider;
		delete spinBox;
		throw std::runtime_error("failed to allocate memory for layout");
	}

	layout->addWidget(label, 0, 0);
	layout->addWidget(slider, 1, 0);
	layout->addWidget(spinBox, 1, 1);
	setLayout(layout);

	connect(spinBox, qOverload<int>(&QSpinBox::valueChanged), slider, &QSlider::setValue);
	connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);
}

void displayTab::applyVibrance(double vibrance){
    QString vibrantXCall = "vibrantX " + name + " " + QString::number(vibrance / 100.0);
    system(vibrantXCall.toUtf8());
	currentVibrance = vibrance;
}

QStringList displayTab::getDisplayNames(){
    QStringList names;

    QProcess xrandr;
    xrandr.start("xrandr");
    xrandr.waitForFinished();

    QStringList res = QString(xrandr.readAll()).split("\n");

    for(int i = 0; i < res.size(); i++){
        QString str = res[i];
        if (str.startsWith("Screen") || str.startsWith("   "))  // Skip Screen X lines and modes list
            continue;
        QStringList splits = str.split(" ");
        if (splits.size() <= 2 || splits.at(1) != "connected")
            continue;

        names.append(splits.at(0));
	}

	return names;
}

int displayTab::getNvidiaSettingsVibrance(const QString &name){
    QProcess vibrantXCall;
    vibrantXCall.start("vibrantX " + name);
    vibrantXCall.waitForFinished();

    QStringList res = QString(vibrantXCall.readAll()).split("\n");

    QString lastLine = res[res.size() - 2];  // the very last line is actually empty
    QStringList splits = lastLine.split(": ");
    if (splits.size() > 1) {
        double d = splits.last().toDouble();
        int rounded = qRound(d * 100);
        return rounded;
    } else {
        return 100;
    }
}

int displayTab::getDefaultVibrance(){
	return spinBox->value();
}

void displayTab::setDefaultVibrance(int value){
	spinBox->setValue(value);
	slider->setValue(value);
}

int displayTab::getCurrentVibrance(){
	return currentVibrance;
}

const QString& displayTab::getName(){
	return name;
}

void displayTab::setName(QString name){
	this->name = name;
}
