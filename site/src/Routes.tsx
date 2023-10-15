import { RouterProvider, createBrowserRouter } from "react-router-dom";
import { GalleryPage, SplashPage, UploadPage } from './pages';

const router = createBrowserRouter([
    {
        path: '/',
        element: <SplashPage />
    },
    {
        path: '/gallery',
        element: <GalleryPage />
    },
    {
        path: '/upload',
        element: <UploadPage />
    }
]);

export const Routes = () => (
    <RouterProvider router={router} />
);
