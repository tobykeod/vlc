/*****************************************************************************
 * folder.c
 *****************************************************************************
 * Copyright (C) 2006 the VideoLAN team
 * $Id$
 *
 * Authors: Antoine Cellerier <dionoea -at- videolan -dot- org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/
#define _GNU_SOURCE
#include <stdlib.h>                                      /* malloc(), free() */

#include <vlc/vlc.h>
#include <vlc_interface.h>
#include <vlc_meta.h>
#include <vlc_playlist.h>
#include <vlc_input.h>
#include <vlc_charset.h>

#ifdef HAVE_SYS_STAT_H
#   include <sys/stat.h>
#endif

#ifndef MAX_PATH
#   define MAX_PATH 250
#endif

/*****************************************************************************
 * Local prototypes
 *****************************************************************************/
static int FindMeta( vlc_object_t * );

/*****************************************************************************
 * Module descriptor
 *****************************************************************************/

vlc_module_begin();
    set_shortname( N_( "Folder" ) );
    set_description( _("Folder meta data") );

    set_capability( "art finder", 10 );
    set_callbacks( FindMeta, NULL );
vlc_module_end();

/*****************************************************************************
 *****************************************************************************/
static int FindMeta( vlc_object_t *p_this )
{
    playlist_t *p_playlist = (playlist_t *)p_this;
    input_item_t *p_item = (input_item_t *)(p_playlist->p_private);
    vlc_bool_t b_have_art = VLC_FALSE;

    int i = 0;
    struct stat a;
    char psz_filename[MAX_PATH];
    char *psz_dir = strdup( p_item->psz_uri );
    char *psz_buf = strrchr( psz_dir, '/' );

    if( !p_item->p_meta ) return VLC_EGENERIC;
    if( psz_buf )
    {
        psz_buf++;
        *psz_buf = '\0';
    }
    else
    {
        *psz_dir = '\0';
    }

    char *psz_path = psz_dir;
    if( !strncmp( psz_path, "file://", 7 ) )
        psz_path += 7;

    for( i = 0; b_have_art == VLC_FALSE && i < 3; i++ )
    {
        switch( i )
        {
            case 0:
            /* Windows Folder.jpg */
            snprintf( psz_filename, MAX_PATH,
                      "file://%sFolder.jpg", psz_path );
            break;

            case 1:
            /* Windows AlbumArtSmall.jpg == small version of Folder.jpg */
            snprintf( psz_filename, MAX_PATH,
                  "file://%sAlbumArtSmall.jpg", psz_path );
            break;

            case 2:
            /* KDE (?) .folder.png */
            snprintf( psz_filename, MAX_PATH,
                  "file://%s.folder.png", psz_path );
            break;
        }

        if( utf8_stat( psz_filename+7, &a ) != -1 )
        {
            vlc_meta_SetArtURL( p_item->p_meta, psz_filename );
            b_have_art = VLC_TRUE;
        }
    }

    free( psz_dir );

    return b_have_art ? VLC_SUCCESS : VLC_EGENERIC;
}
