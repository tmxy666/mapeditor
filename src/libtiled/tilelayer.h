/*
 * tilelayer.h
 * Copyright 2008-2010, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 * Copyright 2009, Jeff Bland <jksb@member.fsf.org>
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

#ifndef TILELAYER_H
#define TILELAYER_H

#include "tiled_global.h"

#include "layer.h"

#include <QString>
#include <QVector>

namespace Tiled {

class Tile;
class Tileset;

/**
 * A tile layer.
 */
class TILEDSHARED_EXPORT TileLayer : public Layer
{
public:
    /**
     * Constructor.
     */
    TileLayer(const QString &name, int x, int y, int width, int height);

    /**
     * Returns the maximum tile size of this layer. Used by the layer
     * rendering code to determine the area that needs to be redrawn.
     */
    QSize maxTileSize() const { return mMaxTileSize; }

    /**
     * Returns whether (x, y) is inside this map layer.
     */
    bool contains(int x, int y) const
    { return x >= 0 && y >= 0 && x < mWidth && y < mHeight; }

    bool contains(const QPoint &point) const
    { return contains(point.x(), point.y()); }

    /**
     * Calculates the region occupied by the tiles of this layer. Similar to
     * Layer::bounds(), but leaves out the regions without tiles.
     */
    QRegion region() const;

    /**
     * Returns the tile at the given coordinates. The coordinates have to
     * be within this layer.
     */
    Tile *tileAt(int x, int y) const
    { return mTiles.at(x + y * mWidth); }

    Tile *tileAt(const QPoint &point) const
    { return tileAt(point.x(), point.y()); }

    /**
     * Sets the tile for the given coordinates.
     */
    void setTile(int x, int y, Tile *tile);

    /**
     * Returns a copy of the area specified by the given \a region. The
     * caller is responsible for the returned tile layer.
     */
    TileLayer *copy(const QRegion &region) const;

    TileLayer *copy(int x, int y, int width, int height) const
    { return copy(QRegion(x, y, width, height)); }

    /**
     * Merges the given \a layer onto this layer at position \a pos. Parts that
     * fall outside of this layer will be lost and empty tiles in the given
     * layer will have no effect.
     */
    void merge(const QPoint &pos, const TileLayer *layer);

    /**
     * Returns whether this tile layer is referencing the given tileset.
     */
    bool referencesTileset(Tileset *tileset) const;

    /**
     * Returns the region of tiles coming from the given \a tileset.
     */
    QRegion tilesetReferences(Tileset *tileset) const;

    /**
     * Removes all references to the given tileset. This sets all tiles on this
     * layer that are from the given tileset to null.
     */
    void removeReferencesToTileset(Tileset *tileset);

    /**
     * Resizes this tile layer to \a size, while shifting all tiles by
     * \a offset.
     *
     * \sa Layer::resize()
     */
    virtual void resize(const QSize &size, const QPoint &offset);

    /**
     * Offsets the objects in this group by \a offset, within \bounds
     * and optionally wraps it.
     *
     * \sa Layer::offset()
     */
    virtual void offset(const QPoint &offset,
                        const QRect &bounds,
                        bool wrapX, bool wrapY);

    /**
     * Returns true if all tiles in the layer are empty.
     */
    bool isEmpty() const;

    virtual Layer *clone() const;

    virtual TileLayer *asTileLayer() { return this; }

protected:
    TileLayer *initializeClone(TileLayer *clone) const;

private:
    QSize mMaxTileSize;
    QVector<Tile*> mTiles;
};

} // namespace Tiled

#endif // TILELAYER_H
