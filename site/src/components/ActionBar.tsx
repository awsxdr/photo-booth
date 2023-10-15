import { AddPhotoAlternate, PhotoLibrary } from "@mui/icons-material"
import { BottomNavigation, BottomNavigationAction, Paper } from "@mui/material"

interface ActionBarProps {
    selectedTab: number
};

export const ActionBar = ({ selectedTab }: ActionBarProps) => {

    return (
        <Paper sx={{ position: 'fixed', bottom: 0, left: 0, right: 0 }} elevation={3}>
            <BottomNavigation showLabels value={selectedTab}>
                <BottomNavigationAction label="View" icon={<PhotoLibrary />} href="/gallery" />
                <BottomNavigationAction label="Upload" icon={<AddPhotoAlternate />} href="/upload" />
            </BottomNavigation>
        </Paper>
    )
}