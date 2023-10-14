import { AddPhotoAlternate, PhotoLibrary } from "@mui/icons-material"
import { BottomNavigation, BottomNavigationAction, Paper } from "@mui/material"
import { SyntheticEvent } from "react"

interface ActionBarProps {
    selectedTab: number
};

export const ActionBar = ({ selectedTab }: ActionBarProps) => {
    const handleValueChanged = (_: SyntheticEvent, _value: number) => {
    };

    return (
        <Paper sx={{ position: 'fixed', bottom: 0, left: 0, right: 0 }} elevation={3}>
            <BottomNavigation showLabels value={selectedTab} onChange={handleValueChanged}>
                <BottomNavigationAction label="View" icon={<PhotoLibrary />} />
                <BottomNavigationAction label="Upload" icon={<AddPhotoAlternate />} />
            </BottomNavigation>
        </Paper>
    )
}