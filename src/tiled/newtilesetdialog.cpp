/*
 * newtilesetdialog.cpp
 * Copyright 2009-2010, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "newtilesetdialog.h"
#include "ui_newtilesetdialog.h"

#include "preferences.h"
#include "tileset.h"
#include "utils.h"

#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <QSettings>

#include <memory>

static const char * const COLOR_ENABLED_KEY = "Tileset/UseTransparentColor";
static const char * const COLOR_KEY = "Tileset/TransparentColor";
static const char * const SPACING_KEY = "Tileset/Spacing";
static const char * const MARGIN_KEY = "Tileset/Margin";

using namespace Tiled;
using namespace Tiled::Internal;

NewTilesetDialog::NewTilesetDialog(const QString &path, QWidget *parent) :
    QDialog(parent),
    mPath(path),
    mUi(new Ui::NewTilesetDialog),
    mNameWasEdited(false),
    mNewTileset(0)
{
    mUi->setupUi(this);

    // Restore previously used settings
    QSettings *s = Preferences::instance()->settings();
    bool colorEnabled = s->value(QLatin1String(COLOR_ENABLED_KEY)).toBool();
    QString colorName = s->value(QLatin1String(COLOR_KEY)).toString();
    QColor color = colorName.isEmpty() ? Qt::magenta : QColor(colorName);
    int spacing = s->value(QLatin1String(SPACING_KEY)).toInt();
    int margin = s->value(QLatin1String(MARGIN_KEY)).toInt();

    mUi->useTransparentColor->setChecked(colorEnabled);
    mUi->colorButton->setColor(color);
    mUi->spacing->setValue(spacing);
    mUi->margin->setValue(margin);

    connect(mUi->browseButton, SIGNAL(clicked()), SLOT(browse()));
    connect(mUi->name, SIGNAL(textEdited(QString)), SLOT(nameEdited(QString)));
    connect(mUi->name, SIGNAL(textChanged(QString)), SLOT(updateOkButton()));
    connect(mUi->image, SIGNAL(textChanged(QString)), SLOT(updateOkButton()));

    updateOkButton();
}

NewTilesetDialog::~NewTilesetDialog()
{
    delete mUi;
}

void NewTilesetDialog::setTileWidth(int width)
{
    mUi->tileWidth->setValue(width);
}

void NewTilesetDialog::setTileHeight(int height)
{
    mUi->tileHeight->setValue(height);
}

Tileset *NewTilesetDialog::createTileset()
{
    if (exec() != QDialog::Accepted)
        return 0;

    return mNewTileset;
}

void NewTilesetDialog::tryAccept()
{
    const QString name = mUi->name->text();
    const QString image = mUi->image->text();
    const bool useTransparentColor = mUi->useTransparentColor->isChecked();
    const QColor transparentColor = mUi->colorButton->color();
    const int tileWidth = mUi->tileWidth->value();
    const int tileHeight = mUi->tileHeight->value();
    const int spacing = mUi->spacing->value();
    const int margin = mUi->margin->value();

    std::auto_ptr<Tileset> tileset(new Tileset(name,
                                               tileWidth, tileHeight,
                                               spacing, margin));

    if (useTransparentColor)
        tileset->setTransparentColor(transparentColor);

    if (!tileset->loadFromImage(image)) {
        QMessageBox::critical(this, tr("Error"),
                              tr("Failed to load tileset image '%1'.")
                              .arg(image));
        return;
    }

    if (tileset->tileCount() == 0) {
        QMessageBox::critical(this, tr("Error"),
                              tr("No tiles found in the tileset image when "
                                 "using the given tile size, margin and "
                                 "spacing!"));
        return;
    }

    // Store settings for next time
    QSettings *s = Preferences::instance()->settings();
    s->setValue(QLatin1String(COLOR_ENABLED_KEY), useTransparentColor);
    s->setValue(QLatin1String(COLOR_KEY), transparentColor.name());
    s->setValue(QLatin1String(SPACING_KEY), spacing);
    s->setValue(QLatin1String(MARGIN_KEY), margin);

    mNewTileset = tileset.release();
    accept();
}

void NewTilesetDialog::browse()
{
    const QString filter = Utils::readableImageFormatsFilter();
    QString f = QFileDialog::getOpenFileName(this, tr("Tileset Image"), mPath,
                                             filter);
    if (!f.isEmpty()) {
        mUi->image->setText(f);
        mPath = f;

        if (!mNameWasEdited)
            mUi->name->setText(QFileInfo(f).completeBaseName());
    }
}

void NewTilesetDialog::nameEdited(const QString &name)
{
    mNameWasEdited = !name.isEmpty();
}

void NewTilesetDialog::updateOkButton()
{
    QPushButton *okButton = mUi->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(!mUi->name->text().isEmpty()
                         && !mUi->image->text().isEmpty());
}
