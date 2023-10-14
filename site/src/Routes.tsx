import { RouterProvider, createBrowserRouter } from "react-router-dom";
import { SplashPage, UploadPage } from './pages';

const router = createBrowserRouter([
    {
        path: '/',
        element: <SplashPage />
    },
    {
        path: '/upload',
        element: <UploadPage />
    }
]);

export const Routes = () => (
    <RouterProvider router={router} />
);
