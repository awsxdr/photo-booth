import { ImageList, ImageListItem } from "@mui/material";
import './Gallery.css';

interface GalleryProps {
    photoNames: string[],
};

export const Gallery = ({ photoNames }: GalleryProps) => {

    return (
        <ImageList variant="masonry">
            {
                photoNames.map(photo => (
                    <ImageListItem key={photo}>
                        <img src={`/photos/thumbnails/${photo}`} className="galleryImage" onClick={() => document.location.href = `/photos/${photo}`} />
                    </ImageListItem>
                ))
            }
        </ImageList>
    )
}