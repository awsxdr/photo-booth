import { createContext, useContext, useState, PropsWithChildren, useEffect } from "react";

interface PhotosContextProps {
    photos: string[];
}

interface PhotosResponse {
    photos: string[]
 };
  
  
const DefaultPhotosContext = (): PhotosContextProps => ({
    photos: [],
});

const PhotosContext = createContext<PhotosContextProps>(DefaultPhotosContext());

export const usePhotosContext = () => useContext(PhotosContext);

export const PhotosContextProvider = ({ children }: PropsWithChildren) => {

    const [photos, setPhotos] = useState<string[]>([]);

    useEffect(() => {
        fetch('/api/photos', {
            method: 'GET'
          }).then((response: Response) => {
            response.json().then((json: PhotosResponse) => {
              const photos = json.photos;
              setPhotos(photos);
            });
          })
    }, [setPhotos]);

    return (
      <PhotosContext.Provider value={{ photos }}>
        { children }
      </PhotosContext.Provider>
    )
}