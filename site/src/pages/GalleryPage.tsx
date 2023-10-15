import { PageTemplate } from "./PageTemplate";
import { Gallery } from "../components/Gallery";
import { usePhotosContext } from "../contexts/PhotosContext";

export const GalleryPage = () => {

    const { photos } = usePhotosContext();

    return (
        <PageTemplate selectedTab={0}>
            <Gallery photoNames={photos} />
        </PageTemplate>
    );
}