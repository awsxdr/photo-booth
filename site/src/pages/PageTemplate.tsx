import { PropsWithChildren } from 'react';
import './PageTemplate.css';
import { ActionBar, Navigation } from '../components';

interface PageTemplateProps {
    selectedTab: number
};

export const PageTemplate = ({ selectedTab, children }: PropsWithChildren<PageTemplateProps>) => (
    <>
        <Navigation />
        { children }
        <ActionBar selectedTab={selectedTab} />
    </>
);