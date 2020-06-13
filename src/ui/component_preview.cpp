#include "component_preview.h"
#include "graphic_component.h"
#include "ui_component_preview.h"
#include "src/ui/mainwindow.h"

#include <QButtonGroup>
#include <QGridLayout>
#include <QScrollBar>

namespace Ohmcha
{

ComponentPreview::ComponentPreview(QWidget *parent)
    : QWidget(parent), ui(new Ui::ComponentPreview)
{
    ui->setupUi(this);

    // Preview setup
    ui->preview->setScene(new QGraphicsScene);
    ui->preview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->preview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->preview->verticalScrollBar()->blockSignals(true);
    ui->preview->horizontalScrollBar()->blockSignals(true);

    // Draw and enable anchor picker
    QButtonGroup *anchorGroup = new QButtonGroup;
    anchorGroup->setExclusive(true);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            anchors[i][j].setText("");
            ui->anchorPicker->addWidget(&anchors[i][j], i, j);
            anchorGroup->addButton(&anchors[i][j], 3*i + j);
        }
    connect(anchorGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this, &ComponentPreview::textAnchorPicked);
    anchors[1][0].setChecked(true);

    // Set callback for text independence picker
    QButtonGroup *textIndependenceGroup = new QButtonGroup;
    textIndependenceGroup->addButton(ui->btnDepend, 0);
    textIndependenceGroup->addButton(ui->btnIndependent, 1);
    ui->btnDepend->setChecked(true);
    connect(textIndependenceGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this, &ComponentPreview::textIndependencePicked);


}

ComponentPreview::~ComponentPreview()
{
    delete ui;
}

void ComponentPreview::setCircuitView(CircuitView *cv)
{
    circuitView = cv;
}

void ComponentPreview::initialize()
{
    // Set callback for double clicking a component TODO: maybe move this somewhere else
    connect(circuitView, &CircuitView::componentSelected,
        this, &ComponentPreview::setComponent);
    initializeNewComponent();
}

void ComponentPreview::synchronize()
{
    ui->editAngle->setText(QString::number(component->rotation()));
    ui->editText->setText(component->getName());
    ui->editTextAngle->setText(QString::number(component->getTextRotation()));
    ui->btnDepend->setChecked(!component->isTextRotationIndependent());
    ui->btnIndependent->setChecked(component->isTextRotationIndependent());

    ui->preview->scene()->update();
}

void ComponentPreview::setComponent(GraphicComponent *component)
{
    ui->preview->scene()->removeItem(this->component);
    ui->preview->scene()->addItem(component);

    this->component = component;
    synchronize();
    newComponent = false;
    ui->btnAdd->setVisible(false);
}

void ComponentPreview::on_btnAdd_clicked()
{
    circuitView->initiateInsertComponent(component, CircuitView::InsertPoints);
    initializeNewComponent();
    connect(circuitView, &CircuitView::componentInserted, this, &ComponentPreview::componentInserted);
}

void ComponentPreview::textAnchorPicked(int id)
{
    component->setTextAnchor(GraphicComponent::Anchor(id));
    updatePreview();
}

void ComponentPreview::textIndependencePicked(int id)
{
    component->setTextRotationIndependent(id);
    updatePreview();
}

void ComponentPreview::componentInserted()
{
    disconnect(circuitView, &CircuitView::componentInserted, this, &ComponentPreview::componentInserted);
}

void ComponentPreview::on_editAngle_textEdited(const QString &s)
{
    //TODO check for input validity in all similar methods
    component->setRotation(s.toFloat());
    updatePreview();
}

void ComponentPreview::on_editText_textEdited(const QString &s)
{
    component->setName(s);
    updatePreview();
}

void ComponentPreview::on_editTextAngle_textEdited(const QString &s)
{
    component->setTextRotation(s.toFloat());
    updatePreview();
}

void ComponentPreview::initializeNewComponent()
{
    if (component == nullptr)
        component = new GraphicResistor(); //TODO generalize
    else
        component = new GraphicResistor(*(GraphicResistor*)component);

    component->setFlag(QGraphicsItem::ItemIsMovable, false);
    component->setFlag(QGraphicsItem::ItemIsSelectable, false);

    synchronize();

    ui->preview->scene()->addItem(component);
    ui->btnAdd->setVisible(true);
}

void ComponentPreview::updatePreview()
{
    ui->preview->scene()->update();
}

}
